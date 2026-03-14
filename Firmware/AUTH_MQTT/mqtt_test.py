#!/usr/bin/env python3
"""
MQTT Gateway Test Script
========================
Tests the ESP32 MQTT gateway by simulating a device.

Protocol recap (from gateway_core.cpp):
  Connect topic  : jrpc/gateway/connect
  RX topic       : jrpc/gateway/rx
  Response topic : jrpc/devices/<device_id>/rx

Connect payload  : {"device_id":"...","nonce":"<24-hex>","ciphertext":"<hex>"}
  Ciphertext     : ChaCha20-Poly1305( key=SHA256(PSK), nonce=12B, aad=b"",
                                      plain={"device_name":"...","device_type":"..."} )
RX payload same shape but plain is a JSON-RPC 2.0 object.

Usage:
  pip install paho-mqtt cryptography
  python mqtt_gateway_test.py [--psk MY_PSK] [--device-id my_sensor]
"""

import argparse
import hashlib
import json
import os
import struct
import sys
import time
import threading

try:
    import paho.mqtt.client as mqtt
except ImportError:
    sys.exit("❌  Missing paho-mqtt — run:  pip install paho-mqtt")

try:
    from cryptography.hazmat.primitives.ciphers.aead import ChaCha20Poly1305
except ImportError:
    sys.exit("❌  Missing cryptography — run:  pip install cryptography")

# ──────────────────────────────────────────────
# Config (matches gateway_config.h)
# ──────────────────────────────────────────────
BROKER      = "broker.hivemq.com"
PORT        = 1883
T_CONNECT   = "jrpc/gateway/connect"
T_RX        = "jrpc/gateway/rx"
T_DEV_RX    = "jrpc/devices/{device_id}/rx"

TIMEOUT_SEC = 10   # seconds to wait for a response


# ──────────────────────────────────────────────
# Crypto helpers (mirror gateway's C code)
# ──────────────────────────────────────────────
def derive_key(psk: str) -> bytes:
    """SHA-256 of the PSK string — matches gw_psk_to_key()."""
    return hashlib.sha256(psk.encode()).digest()


def make_nonce(counter: int) -> bytes:
    """12-byte nonce: 4-byte big-endian counter + 8-byte big-endian timestamp."""
    ts = int(time.time())
    return struct.pack(">I", counter) + struct.pack(">Q", ts)


def encrypt(key: bytes, nonce: bytes, plaintext: bytes) -> bytes:
    """ChaCha20-Poly1305 with no AAD — matches gateway's chacha20_poly1305_encrypt."""
    chacha = ChaCha20Poly1305(key)
    return chacha.encrypt(nonce, plaintext, b"")   # aad = b""


def decrypt(key: bytes, nonce: bytes, ciphertext_with_tag: bytes) -> bytes:
    """ChaCha20-Poly1305 decrypt — raises InvalidTag on auth failure."""
    chacha = ChaCha20Poly1305(key)
    return chacha.decrypt(nonce, ciphertext_with_tag, b"")


# ──────────────────────────────────────────────
# Test class
# ──────────────────────────────────────────────
class GatewayTester:
    def __init__(self, device_id: str, psk: str):
        self.device_id  = device_id
        self.psk        = psk
        self.key        = derive_key(psk)
        self.counter    = 1

        self._responses = []
        self._connected = threading.Event()
        self._got_resp  = threading.Event()
        self._subscribed_ok = False

        self.client = mqtt.Client(client_id=f"gw_tester_{os.getpid()}")
        self.client.on_connect    = self._on_connect
        self.client.on_disconnect = self._on_disconnect
        self.client.on_subscribe  = self._on_subscribe
        self.client.on_message    = self._on_message

    # ── MQTT callbacks ──────────────────────────────────
    def _on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            print(f"  ✅  Connected to {BROKER}:{PORT}")
            resp_topic = T_DEV_RX.format(device_id=self.device_id)
            client.subscribe(resp_topic, qos=1)
            self._connected.set()
        else:
            print(f"  ❌  Connection refused — rc={rc}")

    def _on_disconnect(self, client, userdata, rc):
        if rc != 0:
            print(f"  ⚠️   Unexpected disconnect rc={rc}")

    def _on_subscribe(self, client, userdata, mid, granted_qos):
        self._subscribed_ok = True
        print(f"  ✅  Subscribed to {T_DEV_RX.format(device_id=self.device_id)}")

    def _on_message(self, client, userdata, msg):
        raw = msg.payload.decode(errors="replace")
        print(f"\n  📨  Received on [{msg.topic}]:\n      {raw}")
        self._responses.append({"topic": msg.topic, "payload": raw})
        self._got_resp.set()

    # ── Helpers ─────────────────────────────────────────
    def _build_envelope(self, plaintext: dict) -> dict:
        nonce   = make_nonce(self.counter)
        self.counter += 1
        cipher  = encrypt(self.key, nonce, json.dumps(plaintext).encode())
        return {
            "device_id":  self.device_id,
            "nonce":      nonce.hex(),
            "ciphertext": cipher.hex(),
        }

    def _publish(self, topic: str, payload: dict):
        raw = json.dumps(payload)
        info = self.client.publish(topic, raw, qos=1)
        info.wait_for_publish(timeout=5)
        print(f"  📤  Published to [{topic}]:\n      {raw}")

    # ── Tests ────────────────────────────────────────────
    def run_all(self):
        results = {}

        # ── Test 1: Broker connectivity ──────────────────
        print("\n" + "="*55)
        print("TEST 1 — Broker connectivity")
        print("="*55)
        self.client.connect(BROKER, PORT, keepalive=30)
        self.client.loop_start()
        ok = self._connected.wait(timeout=10)
        results["broker_connect"] = ok
        if not ok:
            print("  ❌  Could not connect to broker (timeout)")
            print("\nPossible causes:")
            print("  • No internet access on this machine")
            print("  • Firewall blocking port 1883")
            print(f"  • Broker {BROKER} is down")
            self.client.loop_stop()
            return results
        print("  ✅  Broker reachable")

        # ── Test 2: Subscription ready ───────────────────
        print("\n" + "="*55)
        print("TEST 2 — Subscription to device response topic")
        print("="*55)
        time.sleep(1)
        results["subscribed"] = self._subscribed_ok
        if not self._subscribed_ok:
            print("  ⚠️   Subscription not confirmed yet (may still work)")
        else:
            print("  ✅  Subscription confirmed")

        # ── Test 3: Connect request ──────────────────────
        print("\n" + "="*55)
        print("TEST 3 — Send device connect request to gateway")
        print("="*55)
        inner = {
            "device_name": "PythonTestDevice",
            "device_type": "tester",
        }
        envelope = self._build_envelope(inner)
        self._got_resp.clear()
        self._publish(T_CONNECT, envelope)
        print(f"\n  ⏳  Waiting {TIMEOUT_SEC}s for gateway to echo connect on device topic…")
        print(f"  (If the ESP32 is running and connected, it will store this as PENDING.)")
        print(f"  (No response is expected until you authorize in the dashboard.)\n")
        got = self._got_resp.wait(timeout=TIMEOUT_SEC)
        results["connect_response"] = got
        if got:
            print("  ✅  Gateway sent a response after connect request!")
        else:
            print("  ℹ️   No response received (expected — device is PENDING until authorized)")

        # ── Test 4: Raw plaintext probe ──────────────────
        print("\n" + "="*55)
        print("TEST 4 — Malformed message probe (gateway error handling)")
        print("="*55)
        bad_payload = json.dumps({
            "device_id": self.device_id,
            "nonce": "this_is_not_hex",
            "ciphertext": "aabbcc",
        })
        self._got_resp.clear()
        info = self.client.publish(T_RX, bad_payload, qos=1)
        info.wait_for_publish(timeout=5)
        print(f"  📤  Sent malformed message to [{T_RX}]")
        print(f"  ⏳  Waiting {TIMEOUT_SEC}s for error response…")
        got = self._got_resp.wait(timeout=TIMEOUT_SEC)
        results["malformed_error_response"] = got
        if got:
            print("  ✅  Gateway responded to malformed message (error handling works)")
        else:
            print("  ⚠️   No response (device not yet known to gateway — expected on first run)")

        # ── Test 5: Echo / ping after approval ──────────
        print("\n" + "="*55)
        print("TEST 5 — Encrypted ping (only works after dashboard authorization)")
        print("="*55)
        rpc_ping = {
            "jsonrpc": "2.0",
            "method":  "ping",
            "params":  {},
            "id":      42,
        }
        envelope2 = self._build_envelope(rpc_ping)
        self._got_resp.clear()
        self._publish(T_RX, envelope2)
        print(f"  ⏳  Waiting {TIMEOUT_SEC}s for pong response…")
        got = self._got_resp.wait(timeout=TIMEOUT_SEC)
        results["ping_response"] = got
        if got:
            resp_raw = self._responses[-1]["payload"]
            # Try to decrypt the response
            try:
                resp_json = json.loads(resp_raw)
                nonce_b   = bytes.fromhex(resp_json["nonce"])
                cipher_b  = bytes.fromhex(resp_json["ciphertext"])
                plain_b   = decrypt(self.key, nonce_b, cipher_b)
                print(f"  ✅  Pong received!  Decrypted: {plain_b.decode()}")
            except Exception as e:
                print(f"  ⚠️   Got response but couldn't decrypt: {e}")
                print(f"      Raw: {resp_raw}")
        else:
            print("  ⚠️   No pong (device must be APPROVED in dashboard first)")

        # ── Summary ──────────────────────────────────────
        self.client.loop_stop()
        self.client.disconnect()
        return results


def print_diagnosis(results: dict):
    print("\n" + "="*55)
    print("DIAGNOSIS")
    print("="*55)
    if not results.get("broker_connect"):
        print("🔴  Cannot reach MQTT broker.")
        print("    → Check internet access and that port 1883 is not firewalled.")
        return

    print("🟢  Broker connection: OK")
    print("🟢  Subscription: " + ("OK" if results.get("subscribed") else "unconfirmed (may still work)"))

    if not results.get("ping_response"):
        print("\n⚠️   Gateway did not respond to ping.")
        print("\nIf the ESP32 serial log shows 'MQTT msg on topic' → gateway received it.")
        print("If the serial log shows nothing → the message was NOT received.\n")
        print("Common fixes:")
        print("  1. Device must be AUTHORIZED in the web dashboard first.")
        print("  2. Both this script and the ESP32 must use the SAME PSK.")
        print("  3. Check the ESP32 serial monitor for any error messages.")
        print("  4. The gateway uses broker.hivemq.com (public) — if it disconnects")
        print("     often, consider a private broker or add a MQTT username/password.")
        print("  5. broker.hivemq.com sometimes silently drops connections;")
        print("     if you see frequent disconnects in the serial log,")
        print("     try mqtt.eclipseprojects.io or test.mosquitto.org instead.")
    else:
        print("🟢  Ping/pong: OK — gateway is fully operational!")


# ──────────────────────────────────────────────
# Entry point
# ──────────────────────────────────────────────
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="MQTT Gateway Test Script")
    parser.add_argument("--broker",    default=BROKER,         help="MQTT broker host")
    parser.add_argument("--port",      default=PORT, type=int, help="MQTT broker port")
    parser.add_argument("--psk",       default="test_psk_1234", help="Pre-shared key (must match dashboard)")
    parser.add_argument("--device-id", default="test_device_01",  help="Device ID to use")
    args = parser.parse_args()

    BROKER = args.broker
    PORT   = args.port

    print(f"\n🔌  MQTT Gateway Tester")
    print(f"    Broker   : {BROKER}:{PORT}")
    print(f"    Device ID: {args.device_id}")
    print(f"    PSK      : {args.psk}")
    print(f"    Key (hex): {derive_key(args.psk).hex()}")

    tester  = GatewayTester(device_id=args.device_id, psk=args.psk)
    results = tester.run_all()
    print_diagnosis(results)
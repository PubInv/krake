# main.py
# Fixed: moved blocking wait OUT of the MQTT callback thread

import json
import time
import random
import argparse
import threading
import hashlib
import hmac
import struct
from datetime import datetime

try:
    import paho.mqtt.client as mqtt
except ImportError:
    print("Install paho-mqtt:  pip install paho-mqtt")
    exit(1)

try:
    from cryptography.hazmat.primitives.ciphers.aead import ChaCha20Poly1305
except ImportError:
    print("Install cryptography:  pip install cryptography")
    exit(1)

# ──────────────────────────────────────────────
#  Configuration
# ──────────────────────────────────────────────
DEFAULT_BROKER    = "broker.hivemq.com"
DEFAULT_PORT      = 1883
DEFAULT_DEVICE_ID = "device_01"
DEFAULT_NAME      = "Test ping device 01"
DEFAULT_TYPE      = "TestDevice"
DEFAULT_PSK       = "test123"

# Topics (must match ESP32 gateway)
T_GATEWAY_RX      = "jrpc/gateway/rx"
T_GATEWAY_CONNECT = "jrpc/gateway/connect"


class SensorDevice:
    DISCONNECTED = "disconnected"
    CONNECTING   = "connecting"
    CONNECTED    = "connected"

    def __init__(self, device_id, name, device_type, broker, port, psk=None):
        self.device_id   = device_id
        self.name        = name
        self.device_type = device_type
        self.broker      = broker
        self.port        = port
        self.rpc_id      = 0
        self.pending_rpc = {}

        self.state    = self.DISCONNECTED
        self.my_topic = f"jrpc/devices/{self.device_id}/rx"

        if psk is None:
            psk = DEFAULT_PSK
        self.enc_key = hashlib.sha256(psk.encode('utf-8')).digest()
        self.counter = 0

        self.client = mqtt.Client(
            callback_api_version=mqtt.CallbackAPIVersion.VERSION1,
            client_id=f"{self.device_id}_{random.randint(0, 0xFFFF):04x}",
            protocol=mqtt.MQTTv311,
        )
        self.client.on_connect    = self._on_connect
        self.client.on_message    = self._on_message
        self.client.on_disconnect = self._on_disconnect

        # Signalled by _handle_encrypted when gateway replies to connect
        self.connect_event = threading.Event()
        self.connect_ok    = False

    # ──────────────────────────────────────────
    #  MQTT Callbacks  (NEVER block here)
    # ──────────────────────────────────────────
    def _on_connect(self, client, userdata, flags, rc):
        if rc != 0:
            self._log(f"MQTT connect failed: rc={rc}")
            return
        self._log(f"MQTT connected to {self.broker}")
        client.subscribe(self.my_topic, qos=1)
        self._log(f"Subscribed to {self.my_topic}")
        # FIX: only *send* the request here — do NOT wait inside the callback.
        self._send_connect_request()

    def _on_disconnect(self, client, userdata, rc):
        self._log(f"MQTT disconnected (rc={rc})")
        self.state = self.DISCONNECTED

    def _on_message(self, client, userdata, msg):
        try:
            payload = json.loads(msg.payload.decode())
        except json.JSONDecodeError:
            return
        if "device_id" in payload and "nonce" in payload and "ciphertext" in payload:
            self._handle_encrypted(payload)

    # ──────────────────────────────────────────
    #  Encryption helpers
    # ──────────────────────────────────────────
    def _build_nonce(self):
        """12-byte nonce: 4-byte big-endian counter + 8-byte big-endian timestamp."""
        self.counter += 1
        return struct.pack('>I', self.counter) + struct.pack('>Q', int(time.time()))

    def _build_auth(self, method: str, timestamp: int) -> str:
        """HMAC-SHA256 signature over '<device_id>:<timestamp>:<method>'.

        Mirrors gw_verify_auth() in gateway_utils.cpp.  The key is the
        derived encryption key (SHA-256 of PSK), not the raw PSK.
        """
        msg = f"{self.device_id}:{timestamp}:{method}".encode()
        return hmac.new(self.enc_key, msg, hashlib.sha256).hexdigest()

    def _encrypt(self, plaintext: bytes):
        """ChaCha20-Poly1305 encrypt. AAD = device_id (matches gateway sendEncrypted)."""
        nonce = self._build_nonce()
        aad   = self.device_id.encode('utf-8')
        ct    = ChaCha20Poly1305(self.enc_key).encrypt(nonce, plaintext, aad)
        return nonce, ct

    def _decrypt(self, nonce: bytes, ciphertext: bytes):
        """ChaCha20-Poly1305 decrypt. AAD = device_id (matches gateway sendEncrypted)."""
        aad = self.device_id.encode('utf-8')
        try:
            return ChaCha20Poly1305(self.enc_key).decrypt(nonce, ciphertext, aad)
        except Exception as e:
            self._log(f"Decryption failed: {e}")
            return None

    # ──────────────────────────────────────────
    #  Connect procedure
    # ──────────────────────────────────────────
    def _send_connect_request(self):
        """Send the encrypted connect envelope — returns immediately."""
        # FIX: reset event so a reconnect attempt doesn't use a stale result
        self.connect_event.clear()
        self.connect_ok = False
        self.state = self.CONNECTING

        method    = "request_connect"
        timestamp = int(time.time())
        inner = {
            "device_name": self.name,
            "device_type": self.device_type,
            "method":      method,
            "timestamp":   timestamp,
            "auth":        self._build_auth(method, timestamp),
        }
        plaintext = json.dumps(inner, separators=(',', ':')).encode()
        nonce, ct = self._encrypt(plaintext)

        outer = {
            "device_id":  self.device_id,
            "nonce":      nonce.hex(),
            "ciphertext": ct.hex(),
        }
        self.client.publish(T_GATEWAY_CONNECT,
                            json.dumps(outer, separators=(',', ':')), qos=1)
        self._log("Connect request sent — waiting for admin approval in dashboard...")

    def wait_for_approval(self, timeout=120.0) -> bool:
        """Block the *caller* (main thread) until the gateway approves or times out."""
        if self.connect_event.wait(timeout):
            if self.connect_ok:
                self.state = self.CONNECTED
                self._log("Approved by gateway ✅")
                return True
            else:
                self.state = self.DISCONNECTED
                self._log("Rejected by gateway ❌")
                return False
        self.state = self.DISCONNECTED
        self._log("Approval timeout ⏰")
        return False

    def _handle_encrypted(self, payload):
        if payload.get("device_id") != self.device_id:
            return
        nonce_hex  = payload.get("nonce")
        cipher_hex = payload.get("ciphertext")
        if not nonce_hex or not cipher_hex:
            return

        plain = self._decrypt(bytes.fromhex(nonce_hex), bytes.fromhex(cipher_hex))
        if plain is None:
            return

        try:
            msg = json.loads(plain.decode())
        except json.JSONDecodeError:
            return

        if msg.get("method") == "connect.response":
            status = msg.get("params", {}).get("status")
            self.connect_ok = (status == "approved")
            self.connect_event.set()          # unblocks wait_for_approval()
        elif "result" in msg:
            self._handle_result(msg)
        elif "error" in msg:
            self._handle_error(msg)

    # ──────────────────────────────────────────
    #  RPC (after approved)
    # ──────────────────────────────────────────
    def send_rpc(self, method, params):
        if self.state != self.CONNECTED:
            self._log("Not connected — cannot send RPC.")
            return
        rpc_id    = self._next_id()
        timestamp = int(time.time())
        inner = {
            "jsonrpc":   "2.0",
            "method":    method,
            "params":    params,
            "id":        rpc_id,
            "timestamp": timestamp,
            "auth":      self._build_auth(method, timestamp),
        }
        plaintext = json.dumps(inner, separators=(',', ':')).encode()
        nonce, ct = self._encrypt(plaintext)
        outer = {
            "device_id":  self.device_id,
            "nonce":      nonce.hex(),
            "ciphertext": ct.hex(),
        }
        self.pending_rpc[rpc_id] = {"method": method, "ts": time.time()}
        self.client.publish(T_GATEWAY_RX,
                            json.dumps(outer, separators=(',', ':')), qos=1)

    def send_ping(self):
        self._log("Pinging gateway...")
        self.send_rpc("ping", {})

    def _handle_result(self, payload):
        rpc_id = payload.get("id", -1)
        result = payload.get("result", {})
        info   = self.pending_rpc.pop(rpc_id, None)
        method = info["method"] if info else "?"
        rtt    = (time.time() - info["ts"]) * 1000 if info else 0
        if method == "ping":
            uptime = result.get("uptime_ms", 0) / 1000
            self._log(f"Pong! Gateway uptime: {uptime:.0f}s  RTT: {rtt:.0f}ms")
        else:
            self._log(f"[RESULT] {method}: {json.dumps(result)}")

    def _handle_error(self, payload):
        rpc_id = payload.get("id", -1)
        error  = payload.get("error", {})
        info   = self.pending_rpc.pop(rpc_id, None)
        method = info["method"] if info else "?"
        self._log(f"ERROR [{method}] code={error.get('code','?')}: {error.get('message','?')}")

    def _next_id(self):
        self.rpc_id += 1
        return self.rpc_id

    def _log(self, msg):
        ts = datetime.now().strftime("%H:%M:%S")
        print(f"[{ts}] [{self.device_id}] {msg}")

    def stop(self):
        self.client.disconnect()
        self.client.loop_stop()
        self._log("Disconnected.")


# ──────────────────────────────────────────────
#  Interactive Mode
# ──────────────────────────────────────────────
def interactive_mode(dev: SensorDevice):
    dev.client.connect(dev.broker, dev.port, keepalive=60)
    dev.client.loop_start()   # network thread — callbacks run here, must not block

    # Wait for admin to approve in the dashboard (blocks main thread, not MQTT thread)
    print("Waiting for admin approval via dashboard (timeout 120s)...")
    if not dev.wait_for_approval(timeout=120.0):
        print("Could not authenticate. Check PSK and gateway.")
        dev.stop()
        return

    print("\n+--- Commands ---------------------+")
    print("|  ping   -- Ping gateway           |")
    print("|  quit   -- Exit                   |")
    print("+-----------------------------------+\n")

    try:
        while True:
            cmd = input(f"[{dev.device_id}|{dev.state}] > ").strip().lower()
            if cmd == "ping":
                dev.send_ping()
            elif cmd in ("quit", "exit", "q"):
                break
            elif cmd == "":
                continue
            else:
                print(f"  Unknown: '{cmd}'")
            time.sleep(0.3)
    except (KeyboardInterrupt, EOFError):
        pass

    dev.stop()


# ──────────────────────────────────────────────
#  CLI Entry Point
# ──────────────────────────────────────────────
if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Device Simulator — ChaCha20-Poly1305 encrypted MQTT",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python main.py --psk test123
  python main.py --device-id sensor_01 --name "Temp sensor" --type sensor --psk secret
        """,
    )
    parser.add_argument("--device-id",  default=DEFAULT_DEVICE_ID)
    parser.add_argument("--name",       default=DEFAULT_NAME)
    parser.add_argument("--type",       default=DEFAULT_TYPE, dest="device_type")
    parser.add_argument("--psk",        default=DEFAULT_PSK)
    parser.add_argument("--broker",     default=DEFAULT_BROKER)
    parser.add_argument("--port",       type=int, default=DEFAULT_PORT)
    args = parser.parse_args()

    dev = SensorDevice(
        args.device_id, args.name, args.device_type,
        args.broker, args.port, args.psk,
    )
    interactive_mode(dev)
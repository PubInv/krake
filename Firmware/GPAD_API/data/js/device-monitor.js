KrakeUI.mountLayout('MQTT Device Monitor');
// Krake / PMD browser MQTT monitor using the same methodology as PMD_Processing_MQTT.pde
// Method: MAC dictionary -> subscribe to <MAC>_ACK, <MAC>_ALM, <MAC>_STATUS, <MAC>_HEARTBEAT -> update last seen.

const OFFLINE_AFTER_MS = 30_000;
const DEVICE_STORAGE_KEY = "krakeDeviceMonitor.macToName.v1";
let client = null;
let messageCount = 0;

const defaultMacToName = {
  "3C61053EE100": "PPG_Lee / MinKrakeLeeE100",
  "F024F9F1B874": "KRAKE_LB0001",
  "142B2FEB1F00": "KRAKE_LB0002",
  "142B2FEB1C64": "KRAKE_LB0003",
  "142B2FEB1E24": "KRAKE_LB0004",
  "F024F9F1B880": "KRAKE_LB0005",
  "F4650BC295C0": "KRAKE_LB0006",
  "F4650BC2959C": "KRAKE_LB0007",
  "F4650BC295AC": "KRAKE_LB0008",
  "F4650BC295D0": "KRAKE_LB0009",
  "F4650BC0B528": "KRAKE_US0014",
  "F4650BC295E8": "KRAKE_US0013",
  "F4650BBB3EE4": "KRAKE_US0012",
  "F4650BC0B530": "KRAKE_US0011",
  "F4650BBB3ED0": "KRAKE_US0010",
  "F4650BBB3ED8": "KRAKE_US0009",
  "F4650BBB3EDC": "KRAKE_US0008",
  "F4650BC0B524": "KRAKE_US0007",
  "F4650BC0B52C": "KRAKE_US0006",
  "ECC9FF7D8EE8": "KRAKE_US0005",
  "ECC9FF7D8EF4": "KRAKE_US0004",
  "ECC9FF7C8C98": "KRAKE_US0003",
  "ECC9FF7D8F00": "KRAKE_US0002",
  "ECC9FF7C8BDC": "KRAKE_US0001",
  "3C61053DF08C": "20240421_USA1",
  "3C6105324EAC": "20240421_USA2",
  "3C61053DF63C": "20240421_USA3",
  "10061C686A14": "20240421_USA4",
  "FCB467F4F74C": "20240421_USA5",
  "CCDBA730098C": "20240421_LEB1",
  "CCDBA730BFD4": "20240421_LEB2",
  "CCDBA7300954": "20240421_LEB3",
  "A0DD6C0EFD28": "20240421_LEB4",
  "10061C684D28": "20240421_LEB5",
  "A0B765F51E28": "MockingKrake_LEB",
  "3C61053DC954": "Not Homework2, Maryville TN"
};

const macToName = loadRegistry();
const devices = Object.fromEntries(Object.entries(macToName).map(([mac, name]) => [mac, buildDevice(mac, name)]));
const subscribedMacs = new Set();

const $ = (id) => document.getElementById(id);

function buildDevice(mac, name) {
  return { mac, name, online: false, lastSeen: null, lastTopic: "—", lastMessage: "No message yet" };
}

function loadRegistry() {
  try {
    const raw = localStorage.getItem(DEVICE_STORAGE_KEY);
    if (!raw) return { ...defaultMacToName };
    const parsed = JSON.parse(raw);
    if (!parsed || typeof parsed !== "object") return { ...defaultMacToName };
    const cleaned = {};
    for (const [mac, name] of Object.entries(parsed)) {
      const normalizedMac = normalizeMac(mac);
      if (normalizedMac && typeof name === "string" && name.trim()) cleaned[normalizedMac] = name.trim();
    }
    return Object.keys(cleaned).length ? cleaned : { ...defaultMacToName };
  } catch (_) {
    return { ...defaultMacToName };
  }
}

function persistRegistry() {
  localStorage.setItem(DEVICE_STORAGE_KEY, JSON.stringify(macToName));
}

function normalizeMac(input) {
  const normalized = String(input || "").replace(/[^A-Fa-f0-9]/g, "").toUpperCase();
  return normalized.length === 12 ? normalized : null;
}

function log(line) {
  const stamp = new Date().toLocaleString();
  $("log").textContent = `[${stamp}] ${line}\n` + $("log").textContent;
}

function setBrokerStatus(online) {
  const el = $("brokerStatus");
  el.textContent = online ? "Broker online" : "Broker offline";
  el.className = `broker ${online ? "online" : "offline"}`;
  $("connectBtn").disabled = online;
  $("disconnectBtn").disabled = !online;
  $("publishBtn").disabled = !online;
}

function normalizeTopic(topic) { return topic.replace(/^\//, ""); }

function macFromTopic(topic) {
  const mac = normalizeTopic(topic).substring(0, 12).toUpperCase();
  return macToName[mac] ? mac : null;
}

function topicSuffix(topic) {
  const parts = normalizeTopic(topic).toUpperCase().split("_");
  return parts.length > 1 ? parts[parts.length - 1] : "";
}

function markSeen(mac, topic, payload) {
  const device = devices[mac];
  if (!device) return;
  const text = String(payload ?? "");
  const suffix = topicSuffix(topic);
  const now = Date.now();
  device.lastTopic = normalizeTopic(topic);
  device.lastMessage = text;
  if (suffix === "ACK" || suffix === "STATUS" || suffix === "HEARTBEAT") {
    device.lastSeen = now;
    const lower = text.toLowerCase();
    device.online = !(lower.includes("offline") || lower.includes("disconnected"));
  }
  messageCount++;
  render();
}

function render() {
  const now = Date.now();
  let online = 0;
  let offline = 0;

  const rows = Object.values(devices).map((d) => {
    const timedOut = !d.lastSeen || now - d.lastSeen > OFFLINE_AFTER_MS;
    const isOnline = d.online && !timedOut;
    isOnline ? online++ : offline++;
    const lastSeen = d.lastSeen ? `${Math.round((now - d.lastSeen) / 1000)}s ago` : "Never";

    return `
      <tr>
        <td><span class="status-pill ${isOnline ? "online" : "offline"}">${isOnline ? "Online" : "Offline"}</span></td>
        <td>${escapeHtml(d.name)}</td>
        <td><code>${d.mac}</code></td>
        <td><code>${escapeHtml(d.lastTopic)}</code></td>
        <td>${lastSeen}</td>
        <td>${escapeHtml(d.lastMessage).slice(0, 120)}</td>
        <td><button class="action-btn secondary remove-device-btn" type="button" data-mac="${d.mac}">Remove</button></td>
      </tr>`;
  }).join("");

  $("deviceTable").innerHTML = rows;
  $("onlineCount").textContent = online;
  $("offlineCount").textContent = offline;
  $("messageCount").textContent = messageCount;
}

function escapeHtml(value) {
  return String(value).replace(/[&<>'"]/g, (ch) => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", "'": "&#39;", '"': "&quot;" }[ch]));
}

function subscribeForMac(mac) {
  if (!client || !client.connected || subscribedMacs.has(mac)) return;
  client.subscribe(`${mac}_ACK`, { qos: 0 });
  client.subscribe(`${mac}_ALM`, { qos: 0 });
  client.subscribe(`${mac}_STATUS`, { qos: 0 });
  client.subscribe(`${mac}_HEARTBEAT`, { qos: 0 });
  subscribedMacs.add(mac);
}

function connect() {
  const url = $("brokerUrl").value.trim();
  const username = $("username").value.trim();
  const password = $("password").value;
  const clientId = `WebMonitor_${Math.random().toString(16).slice(2)}_${Date.now()}`;

  client = mqtt.connect(url, { clientId, username, password, clean: true, reconnectPeriod: 2000, connectTimeout: 8000, keepalive: 15 });

  client.on("connect", () => {
    setBrokerStatus(true);
    log(`Connected as ${clientId}`);
    subscribedMacs.clear();
    for (const mac of Object.keys(macToName)) subscribeForMac(mac);
  });

  client.on("message", (topic, payloadBuffer) => {
    const payload = payloadBuffer.toString();
    const mac = macFromTopic(topic);
    if (!mac) return;
    markSeen(mac, topic, payload);
    log(`${macToName[mac]} | ${normalizeTopic(topic)} | ${payload}`);
  });

  client.on("reconnect", () => log("Reconnecting..."));
  client.on("close", () => setBrokerStatus(false));
  client.on("offline", () => setBrokerStatus(false));
  client.on("error", (err) => log(`MQTT error: ${err.message}`));
}

function disconnect() {
  if (client) client.end(true);
  client = null;
  subscribedMacs.clear();
  setBrokerStatus(false);
  log("Disconnected by user");
}

function publishToAll() {
  if (!client || !client.connected) return;
  const custom = $("customCommand").value.trim();
  const preset = $("commandPreset").value;
  const message = custom || preset;
  const retain = $("retain").checked;
  for (const mac of Object.keys(macToName)) client.publish(`${mac}_ALM`, message, { qos: 0, retain });
  log(`Published to all <MAC>_ALM topics: ${message}`);
}

function addDevice() {
  const name = $("deviceNameInput").value.trim();
  const mac = normalizeMac($("deviceMacInput").value);
  if (!name) return log("Device name is required.");
  if (!mac) return log("MAC address must be 12 hex characters.");
  if (macToName[mac]) return log(`Device ${mac} already exists.`);

  macToName[mac] = name;
  devices[mac] = buildDevice(mac, name);
  persistRegistry();
  subscribeForMac(mac);
  $("deviceNameInput").value = "";
  $("deviceMacInput").value = "";
  log(`Added device ${name} (${mac}).`);
  render();
}

function removeDevice(mac) {
  if (!macToName[mac]) return;
  delete macToName[mac];
  delete devices[mac];
  persistRegistry();
  if (client && client.connected) {
    ["ACK", "ALM", "STATUS", "HEARTBEAT"].forEach((sfx) => client.unsubscribe(`${mac}_${sfx}`));
  }
  subscribedMacs.delete(mac);
  log(`Removed device ${mac}.`);
  render();
}

$("connectBtn").addEventListener("click", connect);
$("disconnectBtn").addEventListener("click", disconnect);
$("publishBtn").addEventListener("click", publishToAll);
$("addDeviceBtn").addEventListener("click", addDevice);
$("deviceTable").addEventListener("click", (event) => {
  const button = event.target.closest(".remove-device-btn");
  if (!button) return;
  removeDevice(button.dataset.mac);
});

setInterval(render, 1000);
render();

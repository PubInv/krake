KrakeUI.mountLayout('MQTT Device Monitor');
// Krake / PMD browser MQTT monitor using the same methodology as PMD_Processing_MQTT.pde
// Method: MAC dictionary -> subscribe to <MAC>_ACK, <MAC>_ALM, <MAC>_STATUS, <MAC>_HEARTBEAT -> update last seen.

const OFFLINE_AFTER_MS = 30_000;
let client = null;
let messageCount = 0;

const macToName = {
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
  "F4650BC0B524": "KRAKE_US0010 / KRAKE_US0007",
  "F4650BBB3ED8": "KRAKE_US0009",
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

const devices = Object.fromEntries(
  Object.entries(macToName).map(([mac, name]) => [mac, {
    mac,
    name,
    online: false,
    lastSeen: null,
    lastTopic: "—",
    lastMessage: "No message yet"
  }])
);

const $ = (id) => document.getElementById(id);

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

function normalizeTopic(topic) {
  return topic.replace(/^\//, "");
}

function macFromTopic(topic) {
  const clean = normalizeTopic(topic);
  const mac = clean.substring(0, 12).toUpperCase();
  return macToName[mac] ? mac : null;
}

function markSeen(mac, topic, payload) {
  const device = devices[mac];
  if (!device) return;

  const text = String(payload ?? "");
  device.lastSeen = Date.now();
  device.lastTopic = normalizeTopic(topic);
  device.lastMessage = text;

  // Last Will / retained offline status support, if firmware publishes it.
  const lower = text.toLowerCase();
  device.online = !(lower.includes("offline") || lower.includes("disconnected"));

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

    const lastSeen = d.lastSeen
      ? `${Math.round((now - d.lastSeen) / 1000)}s ago`
      : "Never";

    return `
      <tr>
        <td><span class="status-pill ${isOnline ? "online" : "offline"}">${isOnline ? "Online" : "Offline"}</span></td>
        <td>${escapeHtml(d.name)}</td>
        <td><code>${d.mac}</code></td>
        <td><code>${escapeHtml(d.lastTopic)}</code></td>
        <td>${lastSeen}</td>
        <td>${escapeHtml(d.lastMessage).slice(0, 120)}</td>
      </tr>`;
  }).join("");

  $("deviceTable").innerHTML = rows;
  $("onlineCount").textContent = online;
  $("offlineCount").textContent = offline;
  $("messageCount").textContent = messageCount;
}

function escapeHtml(value) {
  return String(value).replace(/[&<>'"]/g, (ch) => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", "'": "&#39;", '"': "&quot;"
  }[ch]));
}

function connect() {
  const url = $("brokerUrl").value.trim();
  const username = $("username").value.trim();
  const password = $("password").value;
  const clientId = `WebMonitor_${Math.random().toString(16).slice(2)}_${Date.now()}`;

  client = mqtt.connect(url, {
    clientId,
    username,
    password,
    clean: true,
    reconnectPeriod: 2000,
    connectTimeout: 8000,
    keepalive: 15
  });

  client.on("connect", () => {
    setBrokerStatus(true);
    log(`Connected as ${clientId}`);

    for (const mac of Object.keys(macToName)) {
      // Existing Processing sketch listens to ACK. Extra status/heartbeat topics make online tracking stronger.
      client.subscribe(`${mac}_ACK`, { qos: 0 });
      client.subscribe(`${mac}_ALM`, { qos: 0 });
      client.subscribe(`${mac}_STATUS`, { qos: 0 });
      client.subscribe(`${mac}_HEARTBEAT`, { qos: 0 });
    }
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
  setBrokerStatus(false);
  log("Disconnected by user");
}

function publishToAll() {
  if (!client || !client.connected) return;

  const custom = $("customCommand").value.trim();
  const preset = $("commandPreset").value;
  const message = custom || preset;
  const retain = $("retain").checked;

  for (const mac of Object.keys(macToName)) {
    client.publish(`${mac}_ALM`, message, { qos: 0, retain });
  }
  log(`Published to all <MAC>_ALM topics: ${message}`);
}

$("connectBtn").addEventListener("click", connect);
$("disconnectBtn").addEventListener("click", disconnect);
$("publishBtn").addEventListener("click", publishToAll);

setInterval(render, 1000);
render();

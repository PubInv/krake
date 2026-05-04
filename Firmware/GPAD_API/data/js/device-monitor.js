KrakeUI.mountLayout('MQTT Device Monitor');

const OFFLINE_AFTER_MS = 30_000;
const DEVICE_STORAGE_KEY = 'krakeDeviceMonitor.registry.v2';
let client = null;
let messageCount = 0;

const defaultDevices = [
  ['3C61053EE100', 'PPG_Lee / MinKrakeLeeE100'], ['F024F9F1B874', 'KRAKE_LB0001'],
  ['142B2FEB1F00', 'KRAKE_LB0002'], ['142B2FEB1C64', 'KRAKE_LB0003'], ['142B2FEB1E24', 'KRAKE_LB0004'],
  ['F024F9F1B880', 'KRAKE_LB0005'], ['F4650BC295C0', 'KRAKE_LB0006'], ['F4650BC2959C', 'KRAKE_LB0007'],
  ['F4650BC295AC', 'KRAKE_LB0008'], ['F4650BC295D0', 'KRAKE_LB0009'], ['F4650BC0B528', 'KRAKE_US0014'],
  ['F4650BC295E8', 'KRAKE_US0013'], ['F4650BBB3EE4', 'KRAKE_US0012'], ['F4650BC0B530', 'KRAKE_US0011'],
  ['F4650BBB3ED0', 'KRAKE_US0010'], ['F4650BBB3ED8', 'KRAKE_US0009'], ['F4650BBB3EDC', 'KRAKE_US0008'],
  ['F4650BC0B524', 'KRAKE_US0007'], ['F4650BC0B52C', 'KRAKE_US0006'], ['ECC9FF7D8EE8', 'KRAKE_US0005'],
  ['ECC9FF7D8EF4', 'KRAKE_US0004'], ['ECC9FF7C8C98', 'KRAKE_US0003'], ['ECC9FF7D8F00', 'KRAKE_US0002'],
  ['ECC9FF7C8BDC', 'KRAKE_US0001'], ['3C61053DF08C', '20240421_USA1'], ['3C6105324EAC', '20240421_USA2'],
  ['3C61053DF63C', '20240421_USA3'], ['10061C686A14', '20240421_USA4'], ['FCB467F4F74C', '20240421_USA5'],
  ['CCDBA730098C', '20240421_LEB1'], ['CCDBA730BFD4', '20240421_LEB2'], ['CCDBA7300954', '20240421_LEB3'],
  ['A0DD6C0EFD28', '20240421_LEB4'], ['10061C684D28', '20240421_LEB5'], ['A0B765F51E28', 'MockingKrake_LEB'],
  ['3C61053DC954', 'Not Homework2, Maryville TN']
];

const devices = loadRegistry();
const subscribedMacs = new Set();
const $ = (id) => document.getElementById(id);

function makeDevice(mac, name) {
  return { mac, name, online: false, lastSeen: 0, lastTopic: '—', lastMessage: 'No message yet' };
}

function normalizeMac(input) {
  const normalized = String(input || '').replace(/[^A-Fa-f0-9]/g, '').toUpperCase();
  return normalized.length === 12 ? normalized : null;
}

function loadRegistry() {
  const fromDefaults = Object.fromEntries(defaultDevices.map(([mac, name]) => [mac, makeDevice(mac, name)]));
  try {
    const raw = localStorage.getItem(DEVICE_STORAGE_KEY);
    if (!raw) return fromDefaults;
    const parsed = JSON.parse(raw);
    if (!Array.isArray(parsed)) return fromDefaults;
    const loaded = {};
    for (const row of parsed) {
      if (!Array.isArray(row) || row.length !== 2) continue;
      const mac = normalizeMac(row[0]);
      const name = String(row[1] || '').trim();
      if (mac && name) loaded[mac] = makeDevice(mac, name);
    }
    return Object.keys(loaded).length ? loaded : fromDefaults;
  } catch (_) {
    return fromDefaults;
  }
}

function persistRegistry() {
  const compact = Object.values(devices).map((d) => [d.mac, d.name]);
  localStorage.setItem(DEVICE_STORAGE_KEY, JSON.stringify(compact));
}

function log(line) {
  const stamp = new Date().toLocaleString();
  $('log').textContent = `[${stamp}] ${line}\n` + $('log').textContent;
}

function setBrokerStatus(online) {
  const el = $('brokerStatus');
  el.textContent = online ? 'Broker online' : 'Broker offline';
  el.className = `broker ${online ? 'online' : 'offline'}`;
  $('connectBtn').disabled = online;
  $('disconnectBtn').disabled = !online;
  $('publishBtn').disabled = !online;
}

function macFromTopic(topic) {
  const mac = topic.replace(/^\//, '').substring(0, 12).toUpperCase();
  return devices[mac] ? mac : null;
}

function subscribeForMac(mac) {
  if (!client || !client.connected || subscribedMacs.has(mac)) return;
  ['ACK', 'ALM', 'STATUS', 'HEARTBEAT'].forEach((sfx) => client.subscribe(`${mac}_${sfx}`, { qos: 0 }));
  subscribedMacs.add(mac);
}

function render() {
  const now = Date.now();
  let online = 0;
  const rows = Object.values(devices).map((d) => {
    const isOnline = d.online && d.lastSeen && (now - d.lastSeen <= OFFLINE_AFTER_MS);
    if (isOnline) online++;
    const lastSeen = d.lastSeen ? `${Math.round((now - d.lastSeen) / 1000)}s ago` : 'Never';
    return `<tr>
      <td><span class="status-pill ${isOnline ? 'online' : 'offline'}">${isOnline ? 'Online' : 'Offline'}</span></td>
      <td>${escapeHtml(d.name)}</td>
      <td><code>${d.mac}</code></td>
      <td><code>${escapeHtml(d.lastTopic)}</code></td>
      <td>${lastSeen}</td>
      <td>${escapeHtml(d.lastMessage).slice(0, 120)}</td>
      <td><button class="action-btn secondary remove-device-btn" type="button" data-mac="${d.mac}">Remove</button></td>
    </tr>`;
  }).join('');
  $('deviceTable').innerHTML = rows;
  $('onlineCount').textContent = online;
  $('offlineCount').textContent = Object.keys(devices).length - online;
  $('messageCount').textContent = messageCount;
}

function escapeHtml(v) { return String(v).replace(/[&<>'"]/g, (ch) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', "'": '&#39;', '"': '&quot;' }[ch])); }

function connect() {
  client = mqtt.connect($('brokerUrl').value.trim(), {
    clientId: `WebMonitor_${Math.random().toString(16).slice(2)}_${Date.now()}`,
    username: $('username').value.trim(), password: $('password').value,
    clean: true, reconnectPeriod: 2000, connectTimeout: 8000, keepalive: 15
  });
  client.on('connect', () => {
    setBrokerStatus(true);
    subscribedMacs.clear();
    Object.keys(devices).forEach(subscribeForMac);
  });
  client.on('message', (topic, payloadBuffer) => {
    const mac = macFromTopic(topic);
    if (!mac) return;
    const d = devices[mac];
    const payload = payloadBuffer.toString();
    d.lastTopic = topic.replace(/^\//, '');
    d.lastMessage = payload;
    const suffix = d.lastTopic.toUpperCase().split('_').pop();
    if (suffix === 'ACK' || suffix === 'STATUS' || suffix === 'HEARTBEAT') {
      d.lastSeen = Date.now();
      const lower = payload.toLowerCase();
      d.online = !(lower.includes('offline') || lower.includes('disconnected'));
    }
    messageCount++;
    render();
  });
  client.on('close', () => setBrokerStatus(false));
  client.on('offline', () => setBrokerStatus(false));
  client.on('error', (err) => log(`MQTT error: ${err.message}`));
}

function disconnect() { if (client) client.end(true); client = null; subscribedMacs.clear(); setBrokerStatus(false); }

function addDevice() {
  const mac = normalizeMac($('deviceMacInput').value);
  const name = $('deviceNameInput').value.trim();
  if (!name) return log('Device name is required.');
  if (!mac) return log('MAC address must be 12 hex characters.');
  if (devices[mac]) return log(`Device ${mac} already exists.`);
  devices[mac] = makeDevice(mac, name);
  persistRegistry();
  subscribeForMac(mac);
  $('deviceNameInput').value = '';
  $('deviceMacInput').value = '';
  render();
}

function removeDevice(mac) {
  if (!devices[mac]) return;
  delete devices[mac];
  persistRegistry();
  if (client && client.connected) ['ACK', 'ALM', 'STATUS', 'HEARTBEAT'].forEach((sfx) => client.unsubscribe(`${mac}_${sfx}`));
  subscribedMacs.delete(mac);
  render();
}

function publishToAll() {
  if (!client || !client.connected) return;
  const message = $('customCommand').value.trim() || $('commandPreset').value;
  const retain = $('retain').checked;
  Object.keys(devices).forEach((mac) => client.publish(`${mac}_ALM`, message, { qos: 0, retain }));
}

$('connectBtn').addEventListener('click', connect);
$('disconnectBtn').addEventListener('click', disconnect);
$('publishBtn').addEventListener('click', publishToAll);
$('addDeviceBtn').addEventListener('click', addDevice);
$('deviceTable').addEventListener('click', (event) => {
  const button = event.target.closest('.remove-device-btn');
  if (button) removeDevice(button.dataset.mac);
});

setInterval(render, 1000);
render();

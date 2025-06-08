// setup.js

document.getElementById('wifi-form').addEventListener('submit', async (e) => {
  e.preventDefault();
  const ssid = document.getElementById('ssid').value;
  const password = document.getElementById('password').value;
  const body = JSON.stringify({ ssid, password });
  await fetch('/wifi', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body
  });
  alert("WiFi settings saved. Reboot device to apply.");
});

document.getElementById('mqtt-form').addEventListener('submit', async (e) => {
  e.preventDefault();
  const mqttStatus = await fetch('/mqtt-status')
  const broker = document.getElementById('broker').value;
  const pub = document.getElementById('pub').value;
  const sub = document.getElementById('sub').value;
  const role = document.getElementById('role').value;
  const body = JSON.stringify({ broker, pub, sub, role });
  await fetch('/config', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body
  });
  alert("MQTT settings saved.");
});

document.getElementById('message-form').addEventListener('submit', async (e) => {
  e.preventDefault();
  const S1 = document.getElementById('S1').value;
  const S2 = document.getElementById('S2').value;
  const S3 = document.getElementById('S3').value;
  const S4 = document.getElementById('S4').value;
  const body = JSON.stringify({ S1, S2, S3, S4 });
  await fetch('/messages', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body
  });
  alert("Messages saved.");
});

async function fetchAndFill() {
  try {
    const res1 = await fetch('/config');
    const config = await res1.json();
    document.getElementById('broker').value = config.broker || "";
    document.getElementById('pub').value = config.pub || "";
    document.getElementById('sub').value = config.sub || "";
    document.getElementById('role').value = config.role || "";
  } catch (e) {
    console.warn("/config load failed", e);
  }

  try {
    const res2 = await fetch('/wifi.json');
    const wifi = await res2.json();
    document.getElementById('ssid').value = wifi.ssid || "";
    document.getElementById('password').value = wifi.password || "";
  } catch (e) {
    console.warn("/wifi.json load failed", e);
  }

  try {
    const res3 = await fetch('/messages.json');
    const messages = await res3.json();
    document.getElementById('S1').value = messages.S1 || "";
    document.getElementById('S2').value = messages.S2 || "";
    document.getElementById('S3').value = messages.S3 || "";
    document.getElementById('S4').value = messages.S4 || "";
  } catch (e) {
    console.warn("/messages.json load failed", e);
  }
}

fetchAndFill();
setInterval(loadDevices, 10000); // refresh every 10 seconds

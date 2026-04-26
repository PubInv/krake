
(function () {
  KrakeUI.mountLayout('Public Invention');
  function pad20(str) { return (String(str || '') + '                    ').substring(0, 20); }
  function updateLCD(lines) {
    for (let i = 0; i < 4; i++) KrakeUI.setText('lcd' + i, pad20(lines && lines[i]), '                    ');
  }
  async function fetchLCD() {
    try { const data = await KrakeUI.getJson('/lcd'); if (data.lines) updateLCD(data.lines); } catch (_) {}
  }
  async function fetchStatus() {
    try {
      const data = await KrakeUI.getJson('/status');
      KrakeUI.setText('serial', data.serial || data.mac);
      KrakeUI.setText('ip', data.ip);
      KrakeUI.setText('mac', data.mac);
      KrakeUI.setText('rssi', data.rssi);
      KrakeUI.setText('uptime', data.uptime);
      KrakeUI.setText('mqtt', data.mqtt);
      KrakeUI.setText('firmware', data.firmware);
      KrakeUI.setText('compiled', data.compiled);
      KrakeUI.setText('deviceUrl', data.url);
    } catch (_) {}
  }
  fetchLCD(); fetchStatus();
  setInterval(fetchLCD, 1000);
  setInterval(fetchStatus, 2000);
})();


(function () {
  KrakeUI.mountLayout('Factory Test / Developer Monitor');
  async function fetchStatus() {
    try { const data = await KrakeUI.getJson('/status'); KrakeUI.setText('serialPort', data.serialPort); KrakeUI.setText('serialBaud', data.serialBaud); }
    catch (_) { KrakeUI.setText('uartText', 'UART status fetch failed'); }
  }
  async function fetchSerialMonitor() {
    try {
      const res = await fetch('/serial-monitor', { cache: 'no-store' });
      if (!res.ok) return;
      const text = await res.text();
      const uartText = KrakeUI.byId('uartText');
      const nearBottom = uartText.scrollTop + uartText.clientHeight >= uartText.scrollHeight - 8;
      uartText.textContent = text || '(No serial output yet)';
      if (nearBottom) uartText.scrollTop = uartText.scrollHeight;
    } catch (_) { KrakeUI.setText('uartText', 'Serial monitor fetch failed'); }
  }
  fetchStatus(); fetchSerialMonitor(); setInterval(fetchStatus, 1000); setInterval(fetchSerialMonitor, 300);
})();

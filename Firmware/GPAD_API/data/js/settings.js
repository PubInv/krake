
(function () {
  KrakeUI.mountLayout('Settings');
  async function loadWifi() {
    const data = await KrakeUI.getJson('/wifi');
    KrakeUI.byId('ssid').value = data.ssid || '';
    KrakeUI.setText('storedSsid', data.hasStored ? (data.ssid || '(hidden)') : 'None');
    KrakeUI.setText('storedCount', typeof data.count === 'number' ? String(data.count) : '0');
  }
  async function saveWifi() {
    const ssid = KrakeUI.byId('ssid').value.trim();
    const password = KrakeUI.byId('password').value;
    if (!ssid) return KrakeUI.showMessage('SSID is required.', true);
    if (!password || !password.trim()) return KrakeUI.showMessage('Password is required.', true);
    try { await KrakeUI.postForm('/wifi', { ssid, password }); KrakeUI.showMessage('WiFi credentials saved. Device will retry all saved networks on boot.'); await loadWifi(); }
    catch (e) { KrakeUI.showMessage('Failed to save WiFi: ' + e.message, true); }
  }
  async function refreshSettings() {
    const data = await KrakeUI.getJson('/settings-data');
    KrakeUI.byId('broker').value = data.broker || '';
    KrakeUI.byId('role').value = data.role || 'Krake';
    KrakeUI.byId('topics').value = data.extraTopics || '';
    KrakeUI.byId('publishTopics').value = data.publishTopics || '';
    KrakeUI.byId('subscribeTopic').value = data.subscribeTopic || '';
    KrakeUI.byId('publishTopic').value = data.publishTopic || '';
    KrakeUI.setText('muteStatus', data.muted ? 'Muted' : 'Unmuted');
    KrakeUI.setText('alarmTopic', data.subscribeTopic || '-');
    KrakeUI.setText('ackTopic', data.publishTopic || '-');
  }
  async function resetWifi() {
    if (!confirm('This will clear WiFi credentials and restart KRAKE. Continue?')) return;
    try { await KrakeUI.postForm('/settings/wifi/reset', {}); KrakeUI.showMessage('WiFi reset started. Device will restart shortly.'); }
    catch (e) { KrakeUI.showMessage('WiFi reset failed: ' + e.message, true); }
  }
  async function setMuted(muted) {
    try { await KrakeUI.postForm('/settings/mute', { muted: muted ? '1' : '0' }); KrakeUI.showMessage(muted ? 'KRAKE muted.' : 'KRAKE unmuted.'); await refreshSettings(); }
    catch (e) { KrakeUI.showMessage('Failed to update mute status: ' + e.message, true); }
  }

  async function saveMqttConfig() {
    try {
      const role = KrakeUI.byId('role').value;
      const broker = KrakeUI.byId('broker').value.trim();
      const subscribeTopic = KrakeUI.byId('subscribeTopic').value.trim();
      const publishTopic = KrakeUI.byId('publishTopic').value.trim();
      const subscribeTopics = KrakeUI.byId('topics').value;
      const publishTopics = KrakeUI.byId('publishTopics').value;
      await KrakeUI.postForm('/config', { role, broker, subscribeTopic, publishTopic, subscribeTopics, publishTopics, publishDefaultTopic: publishTopic });
      KrakeUI.showMessage('MQTT config updated and saved to /mqtt.json.');
      await refreshSettings();
    } catch (e) { KrakeUI.showMessage('Failed to save MQTT config: ' + e.message, true); }
  }
  window.saveWifi = saveWifi; window.loadWifi = () => loadWifi().catch(e => KrakeUI.showMessage('Unable to load WiFi settings: ' + e.message, true));
  window.resetWifi = resetWifi; window.setMuted = setMuted; window.saveMqttConfig = saveMqttConfig;
  Promise.all([loadWifi(), refreshSettings()]).catch(e => KrakeUI.showMessage(e.message, true));
})();

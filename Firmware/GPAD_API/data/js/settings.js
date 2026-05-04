
(function () {
  KrakeUI.mountLayout('Settings');
  function setInputValue(id, value) {
    const node = KrakeUI.byId(id);
    if (node) node.value = value || '';
  }
  function getInputValue(id) {
    const node = KrakeUI.byId(id);
    return node ? node.value : '';
  }
  async function loadWifi() {
    const data = await KrakeUI.getJson('/wifi');
    setInputValue('ssid', data.ssid || '');
    KrakeUI.setText('storedSsid', data.hasStored ? (data.ssid || '(hidden)') : 'None');
    KrakeUI.setText('storedCount', typeof data.count === 'number' ? String(data.count) : '0');
  }
  async function saveWifi() {
    const ssid = getInputValue('ssid').trim();
    const password = getInputValue('password');
    if (!ssid) return KrakeUI.showMessage('SSID is required.', true);
    if (!password || !password.trim()) return KrakeUI.showMessage('Password is required.', true);
    try { await KrakeUI.postForm('/wifi', { ssid, password }); KrakeUI.showMessage('WiFi credentials saved. Device will retry all saved networks on boot.'); await loadWifi(); }
    catch (e) { KrakeUI.showMessage('Failed to save WiFi: ' + e.message, true); }
  }
  async function refreshSettings() {
    const data = await KrakeUI.getJson('/settings-data');
    setInputValue('broker', data.broker || '');
    setInputValue('role', data.role || 'Krake');
    setInputValue('topics', data.extraTopics || '');
    setInputValue('publishTopics', data.publishTopics || '');
    setInputValue('subscribeTopic', data.subscribeTopic || '');
    setInputValue('publishTopic', data.publishTopic || '');
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
      const role = 'Krake';
      const broker = getInputValue('broker').trim();
      const subscribeTopic = getInputValue('subscribeTopic').trim();
      const publishTopic = getInputValue('publishTopic').trim();
      const subscribeTopics = getInputValue('topics');
      const publishTopics = getInputValue('publishTopics');
      await KrakeUI.postForm('/config', { role, broker, subscribeTopic, publishTopic, subscribeTopics, publishTopics, publishDefaultTopic: publishTopic });
      KrakeUI.showMessage('MQTT config updated and saved to /mqtt.json.');
      await refreshSettings();
    } catch (e) { KrakeUI.showMessage('Failed to save MQTT config: ' + e.message, true); }
  }
  window.saveWifi = saveWifi; window.loadWifi = () => loadWifi().catch(e => KrakeUI.showMessage('Unable to load WiFi settings: ' + e.message, true));
  window.resetWifi = resetWifi; window.setMuted = setMuted; window.saveMqttConfig = saveMqttConfig;
  Promise.all([loadWifi(), refreshSettings()]).catch(e => KrakeUI.showMessage(e.message, true));
})();

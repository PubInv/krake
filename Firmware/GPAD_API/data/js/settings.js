
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
  function syncBrokerProfileFields() {
    const custom = getInputValue('brokerProfile') === 'custom';
    ['customBroker', 'customUser', 'customPassword'].forEach((id) => { const node = KrakeUI.byId(id); if (node) node.disabled = !custom; });
  }
  async function refreshSettings() {
    const data = await KrakeUI.getJson('/settings-data');
    setInputValue('brokerProfile', data.brokerProfile || 'krake');
    setInputValue('customBroker', data.customBroker || '');
    setInputValue('customUser', data.customUser || '');
    setInputValue('customPassword', '');
    syncBrokerProfileFields();
    KrakeUI.setText('activeBroker', data.broker || KrakeUI.mqttBroker.hostname);
    KrakeUI.setText('connectedBroker', data.connectedBroker || 'none');
    KrakeUI.setText('customPasswordStatus', data.customPasswordStored ? 'Stored (write-only)' : 'None');
    setInputValue('role', data.role || 'Krake');
    setInputValue('topics', data.extraTopics || '');
    setInputValue('publishTopics', data.publishTopics || '');
    setInputValue('subscribeTopic', data.publishTopic || '');
    setInputValue('publishTopic', data.subscribeTopic || '');
    setInputValue('volume', String(data.volume || 20));
    setInputValue('muteTimeoutMinutes', String(data.muteTimeoutMinutes || 5));
    setInputValue('alarmRepeatSeconds', String(data.alarmRepeatSeconds || 10));
    KrakeUI.setText('muteStatus', data.muted ? 'Muted' : 'Unmuted');
    KrakeUI.setText('alarmTopic', data.publishTopic || '-');
    KrakeUI.setText('ackTopic', data.subscribeTopic || '-');
  }
  async function resetWifi() {
    if (!confirm('This will clear WiFi credentials and restart KRAKE. Continue?')) return;
    try { await KrakeUI.postForm('/settings/wifi/reset', {}); KrakeUI.showMessage('WiFi reset started. Device will restart shortly.'); }
    catch (e) { KrakeUI.showMessage('WiFi reset failed: ' + e.message, true); }
  }
  async function saveSoundSettings() {
    const volume = Number(getInputValue('volume'));
    const muteTimeoutMinutes = Number(getInputValue('muteTimeoutMinutes'));
    const alarmRepeatSeconds = Number(getInputValue('alarmRepeatSeconds'));
    if (!Number.isInteger(volume) || volume < 1 || volume > 100) return KrakeUI.showMessage('Volume must be between 1 and 100%.', true);
    if (!Number.isInteger(muteTimeoutMinutes) || muteTimeoutMinutes < 1 || muteTimeoutMinutes > 60) return KrakeUI.showMessage('Mute duration must be between 1 and 60 minutes.', true);
    if (!Number.isInteger(alarmRepeatSeconds) || alarmRepeatSeconds < 1 || alarmRepeatSeconds > 300) return KrakeUI.showMessage('Alarm repeat delay must be between 1 and 300 seconds.', true);
    try { await KrakeUI.postForm('/settings/sound', { volume, muteTimeoutMinutes, alarmRepeatSeconds }); KrakeUI.showMessage('Sound defaults saved for future restarts.'); await refreshSettings(); }
    catch (e) { KrakeUI.showMessage('Failed to save sound defaults: ' + e.message, true); }
  }
  async function setMuted(muted) {
    try { await KrakeUI.postForm('/settings/mute', { muted: muted ? '1' : '0' }); KrakeUI.showMessage(muted ? 'KRAKE muted.' : 'KRAKE unmuted.'); await refreshSettings(); }
    catch (e) { KrakeUI.showMessage('Failed to update mute status: ' + e.message, true); }
  }

  async function saveMqttConfig() {
    try {
      const role = 'Krake';
      const brokerProfile = getInputValue('brokerProfile');
      const customBroker = getInputValue('customBroker').trim();
      const customUser = getInputValue('customUser').trim();
      const customPassword = getInputValue('customPassword');
      if (brokerProfile === 'custom' && !customBroker) return KrakeUI.showMessage('Custom broker host is required.', true);
      const subscribeTopicUi = getInputValue('subscribeTopic').trim();
      const publishTopicUi = getInputValue('publishTopic').trim();
      const subscribeTopics = getInputValue('topics');
      const publishTopics = getInputValue('publishTopics');
      const config = { role, brokerProfile, subscribeTopic: publishTopicUi, publishTopic: subscribeTopicUi, subscribeTopics, publishTopics, publishDefaultTopic: publishTopicUi };
      if (brokerProfile === 'custom') Object.assign(config, { customBroker, customUser });
      if (customPassword) config.customPassword = customPassword;
      await KrakeUI.postForm('/config', config);
      KrakeUI.showMessage('MQTT broker and topic settings updated and saved.');
      await refreshSettings();
    } catch (e) { KrakeUI.showMessage('Failed to save MQTT config: ' + e.message, true); }
  }
  window.saveWifi = saveWifi; window.loadWifi = () => loadWifi().catch(e => KrakeUI.showMessage('Unable to load WiFi settings: ' + e.message, true));
  window.resetWifi = resetWifi; window.saveSoundSettings = saveSoundSettings; window.setMuted = setMuted; window.saveMqttConfig = saveMqttConfig; window.syncBrokerProfileFields = syncBrokerProfileFields;
  Promise.all([loadWifi(), refreshSettings()]).catch(e => KrakeUI.showMessage(e.message, true));
})();

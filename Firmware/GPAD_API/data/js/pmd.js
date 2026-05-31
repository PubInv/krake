
(function () {
  KrakeUI.mountLayout('PMD Web UI');
  function selectedTopics() {
    const node = KrakeUI.byId('topicSelect');
    return node ? Array.from(node.selectedOptions).map((o) => o.value.trim()).filter(Boolean) : [];
  }
  function showSelectedTopics() {
    const topics = selectedTopics();
    KrakeUI.setText('selectedTopic', topics.length ? topics.join(', ') : '-');
  }
  async function loadTopics() {
    const data = await KrakeUI.getJson('/settings-data');
    KrakeUI.setText('deviceRole', data.role || 'Krake');
    KrakeUI.setText('brokerName', data.broker || 'public.cloud.shiftr.io');
    const list = KrakeUI.getPublishTopics(data);
    const select = KrakeUI.byId('topicSelect');
    select.innerHTML = '';
    if (!list.length) {
      const opt = document.createElement('option'); opt.value = ''; opt.textContent = 'No configured publish topics'; select.appendChild(opt);
    } else {
      list.forEach((topic) => { const opt = document.createElement('option'); opt.value = topic; opt.textContent = topic; select.appendChild(opt); });
      Array.from(select.options).forEach((o, index) => { if (o.value === data.publishDefaultTopic || (!data.publishDefaultTopic && index === 0)) o.selected = true; });
    }
    const manualTopic = KrakeUI.byId('publishTopic');
    if (manualTopic && !manualTopic.value.trim()) manualTopic.value = data.publishDefaultTopic || data.publishTopic || list[0] || '';
    showSelectedTopics();
  }
  async function publishEmergency(level) {
    const topics = selectedTopics();
    if (!topics.length) { KrakeUI.setText('result', 'Select at least one topic.'); return; }
    const payload = 'a' + String(level) + ' MessageFromProcessing_PMD: a' + String(level);
    const outcomes = [];
    for (const topic of topics) {
      try { await KrakeUI.postForm('/broker-console/publish', { topic, payload }); outcomes.push('OK: ' + topic); }
      catch (e) { outcomes.push('ERR: ' + topic + ' (' + e.message + ')'); }
    }
    KrakeUI.setText('result', outcomes.join(' | '));
    KrakeUI.showMessage('Sent level ' + level + ' to ' + topics.length + ' topic(s).', outcomes.some((o) => o.startsWith('ERR')));
  }
  async function publishManualMessage() {
    const topic = KrakeUI.byId('publishTopic').value.trim();
    const payload = KrakeUI.byId('publishPayload').value.trim();
    if (!topic || !payload) return KrakeUI.showMessage('Topic and payload are required.', true, 'publishResult');
    try { const res = await KrakeUI.postForm('/broker-console/publish', { topic, payload }); KrakeUI.showMessage(res || 'Published.', false, 'publishResult'); }
    catch (e) { KrakeUI.showMessage(e.message, true, 'publishResult'); }
  }
  window.publishEmergency = publishEmergency;
  KrakeUI.byId('topicSelect').addEventListener('change', showSelectedTopics);
  KrakeUI.byId('btnSendMessage').addEventListener('click', publishManualMessage);
  loadTopics().catch((e) => KrakeUI.showMessage(e.message, true));
})();

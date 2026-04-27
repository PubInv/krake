(function () {
  if (window.KrakeUI) KrakeUI.mountLayout('Broker Console');
  const state = {
    pollHandle: null
  };

  function byId(id) {
    return document.getElementById(id);
  }

  function toggleMenu() {
    byId('sideMenu').classList.toggle('open');
  }

  function showMessage(message, isError = false) {
    const node = byId('message');
    node.textContent = message;
    node.style.color = isError ? '#b00020' : '#146620';
  }

  function setPublishResult(message, isError = false) {
    const node = byId('publishResult');
    node.textContent = message;
    node.style.color = isError ? '#b00020' : '#146620';
  }

  function setWebMessageResult(message, isError = false) {
    const node = byId('webMessageResult');
    if (!node) return;
    node.textContent = message;
    node.style.color = isError ? '#b00020' : '#146620';
  }

  async function postForm(url, bodyObj) {
    const response = await fetch(url, {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: new URLSearchParams(bodyObj)
    });

    const text = await response.text();
    if (!response.ok) {
      throw new Error(text || ('HTTP ' + response.status));
    }

    return text;
  }

  function parsePrimaryId(topic) {
    if (!topic) return '-';
    const match = topic.match(/^([A-Za-z0-9]+)_(ACK|ALM)$/i);
    return match ? match[1] : topic;
  }

  function updatePublishTopicsUi(publishDefaultTopic) {
    const publishTopic = byId('publishTopic');
    if (publishTopic && !publishTopic.value.trim() && publishDefaultTopic) {
      publishTopic.value = publishDefaultTopic;
    }
  }

  async function refreshBrokerData() {
    try {
      const response = await fetch('/broker-console/data', { cache: 'no-store' });
      if (!response.ok) {
        throw new Error('Unable to load broker data');
      }

      const data = await response.json();
      const brokerName = byId('brokerName');
      if (brokerName) {
        brokerName.textContent = data.broker || brokerName.textContent;
      }
      const roleNode = byId('deviceRole');
      if (roleNode) {
        roleNode.textContent = data.role || 'Krake';
      }
      updatePublishTopicsUi(data.publishDefaultTopic || '');
      const publishTopic = byId('publishTopic');
      if (publishTopic && !publishTopic.value.trim() && data.subscribeAlarmTopic) {
        publishTopic.value = data.subscribeAlarmTopic;
      }
    } catch (error) {
      showMessage('Broker data refresh failed: ' + error.message, true);
    }
  }

  async function publishMessage() {
    const topic = byId('publishTopic').value.trim();
    const payload = byId('publishPayload').value.trim();

    if (!topic || !payload) {
      setPublishResult('Topic and payload are required.', true);
      return;
    }

    try {
      const result = await postForm('/broker-console/publish', { topic, payload });
      setPublishResult(result || 'Published');
      showMessage('MQTT message published.');
    } catch (error) {
      setPublishResult(error.message, true);
      showMessage('Publish failed: ' + error.message, true);
    }
  }

  async function sendWebMessage() {
    const topic = byId('publishTopic').value.trim();
    const textInput = byId('webMessageText');
    const messageText = textInput ? textInput.value.trim() : '';

    if (!topic) {
      setWebMessageResult('Topic is required.', true);
      return;
    }

    if (!messageText) {
      setWebMessageResult('Message text is required.', true);
      return;
    }

    const payload = 'a0 ' + messageText;

    try {
      const result = await postForm('/broker-console/publish', { topic, payload });
      setWebMessageResult(result || 'Published');
      showMessage('Webpage message published.');
      byId('publishPayload').value = payload;
    } catch (error) {
      setWebMessageResult(error.message, true);
      showMessage('Webpage message publish failed: ' + error.message, true);
    }
  }

  function setupTemplates() {
    const btnUseAlm = byId('btnUseAlm');
    if (btnUseAlm) btnUseAlm.addEventListener('click', () => {
      const base = parsePrimaryId(byId('publishTopic').value.trim()).replace(/[^A-Za-z0-9]/g, '');
      byId('publishTopic').value = (base || 'AABBCCDDEEFF') + '_ALM';
    });

    const btnUseAck = byId('btnUseAck');
    if (btnUseAck) btnUseAck.addEventListener('click', () => {
      const base = parsePrimaryId(byId('publishTopic').value.trim()).replace(/[^A-Za-z0-9]/g, '');
      byId('publishTopic').value = (base || 'AABBCCDDEEFF') + '_ACK';
    });

    const btnAlarmTemplate = byId('btnAlarmTemplate');
    if (btnAlarmTemplate) btnAlarmTemplate.addEventListener('click', () => {
      const timestamp = new Date().toISOString().replace(/[-:TZ.]/g, '').slice(0, 14);
      byId('publishPayload').value = 'a3 ' + timestamp + ' Test alarm message';
    });

    const btnInfoTemplate = byId('btnInfoTemplate');
    if (btnInfoTemplate) btnInfoTemplate.addEventListener('click', () => {
      const timestamp = new Date().toISOString().replace(/[-:TZ.]/g, '').slice(0, 14);
      byId('publishPayload').value = 'a0 ' + timestamp + ' Test info message';
    });

    const btnMuteTemplate = byId('btnMuteTemplate');
    if (btnMuteTemplate) btnMuteTemplate.addEventListener('click', () => {
      byId('publishPayload').value = 'S';
    });

    const btnUnmuteTemplate = byId('btnUnmuteTemplate');
    if (btnUnmuteTemplate) btnUnmuteTemplate.addEventListener('click', () => {
      byId('publishPayload').value = 'U';
    });
  }

  function setupUiActions() {
    const menuToggle = byId('menuToggle');
    if (menuToggle) {
      menuToggle.addEventListener('click', toggleMenu);
    }

    const btnSendMessage = byId('btnSendMessage');
    if (btnSendMessage) {
      btnSendMessage.addEventListener('click', publishMessage);
    }
    const sendWebBtn = byId('btnSendWebMessage');
    if (sendWebBtn) {
      sendWebBtn.addEventListener('click', sendWebMessage);
    }

    setupTemplates();
  }

  async function init() {
    setupUiActions();
    await refreshBrokerData();

    state.pollHandle = setInterval(refreshBrokerData, 2000);
  }

  window.addEventListener('beforeunload', () => {
    if (state.pollHandle) {
      clearInterval(state.pollHandle);
      state.pollHandle = null;
    }
  });

  init().catch((error) => {
    showMessage('Broker console init failed: ' + error.message, true);
  });
})();

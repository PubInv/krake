(function () {
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

  function normalizeTopicList(raw) {
    return raw
      .split(',')
      .map((topic) => topic.trim())
      .filter(Boolean)
      .join(',');
  }

  function parsePrimaryId(topic) {
    if (!topic) return '-';
    const match = topic.match(/^([A-Za-z0-9]+)_(ACK|ALM)$/i);
    return match ? match[1] : topic;
  }

  function ageText(seconds) {
    if (seconds == null || seconds < 0) return '-';
    if (seconds < 60) return seconds + 's';
    const minutes = Math.floor(seconds / 60);
    const remainder = seconds % 60;
    return minutes + 'm ' + remainder + 's';
  }

  function renderKrakeTable(krakes) {
    const tbody = byId('krakeTableBody');
    const krakeCount = byId('krakeCount');
    if (!tbody) {
      if (krakeCount) {
        krakeCount.textContent = String(Array.isArray(krakes) ? krakes.length : 0);
      }
      return;
    }

    if (!krakes || krakes.length === 0) {
      tbody.innerHTML = '<tr><td colspan="9">No tracked devices yet.</td></tr>';
      if (krakeCount) {
        krakeCount.textContent = '0';
      }
      return;
    }

    const rows = krakes.map((krake) => {
      const online = !!krake.online;
      const topicActive = !!krake.topicParticipant;
      const rowClass = online ? 'row-online' : 'row-offline';
      const onlinePill = online
        ? '<span class="pill ok">ONLINE</span>'
        : '<span class="pill bad">OFFLINE</span>';
      const topicPill = topicActive
        ? '<span class="pill ok">ACTIVE</span>'
        : '<span class="pill bad">IDLE</span>';

      return [
        '<tr class="' + rowClass + '">',
        '<td>' + parsePrimaryId(krake.id) + '</td>',
        '<td><code>' + (krake.id || '-') + '</code></td>',
        '<td>' + onlinePill + '</td>',
        '<td>' + topicPill + '</td>',
        '<td>' + (Number.isFinite(krake.rssi) ? krake.rssi : '-') + '</td>',
        '<td><code>' + (krake.status || '-') + '</code></td>',
        '<td><code>' + (krake.lastTopic || '-') + '</code></td>',
        '<td>' + ageText(krake.secondsSinceStatus) + '</td>',
        '<td>' + ageText(krake.secondsSinceTopic) + '</td>',
        '</tr>'
      ].join('');
    });

    tbody.innerHTML = rows.join('');
    if (krakeCount) {
      krakeCount.textContent = String(krakes.length);
    }
  }

  function updateWatchUi(watchedTopics) {
    const normalized = normalizeTopicList(watchedTopics || '');
    const watchTopic = byId('watchTopic');
    if (watchTopic) {
      watchTopic.value = normalized;
    }
    const watchTopicCurrent = byId('watchTopicCurrent');
    if (watchTopicCurrent) {
      watchTopicCurrent.textContent = normalized || '-';
    }
  }

  function updatePublishTopicsUi(publishTopics, publishDefaultTopic) {
    const normalized = normalizeTopicList(publishTopics || '');
    const list = byId('publishTopicList');
    if (list) {
      list.value = normalized;
    }
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
      updateWatchUi(data.watchedTopics || data.watchedTopic || '');
      renderKrakeTable(Array.isArray(data.krakes) ? data.krakes : []);
      const brokerName = byId('brokerName');
      if (brokerName) {
        brokerName.textContent = data.broker || brokerName.textContent;
      }
      const roleNode = byId('deviceRole');
      if (roleNode) {
        roleNode.textContent = data.role || 'Krake';
      }
      updatePublishTopicsUi(data.publishTopics || '', data.publishDefaultTopic || '');
      const publishTopic = byId('publishTopic');
      if (publishTopic && !publishTopic.value.trim() && data.subscribeAlarmTopic) {
        publishTopic.value = data.subscribeAlarmTopic;
      }
    } catch (error) {
      showMessage('Broker data refresh failed: ' + error.message, true);
    }
  }

  function appendWatchTopic(topic) {
    const watchInput = byId('watchTopic');
    if (!watchInput) return;
    const current = normalizeTopicList(watchInput.value);
    const parts = current ? current.split(',') : [];

    if (!parts.includes(topic)) {
      parts.push(topic);
    }

    watchInput.value = parts.join(',');
  }

  async function startWatchingTopics() {
    const watchTopic = byId('watchTopic');
    if (!watchTopic) {
      showMessage('Topic monitor controls are not available on this page.', true);
      return;
    }
    const topics = normalizeTopicList(watchTopic.value);
    if (!topics) {
      showMessage('Please provide at least one topic to watch.', true);
      return;
    }

    try {
      await postForm('/broker-console/topic', { topics });
      showMessage('Watch topics updated.');
      await refreshBrokerData();
    } catch (error) {
      showMessage('Failed to update watch topics: ' + error.message, true);
    }
  }

  async function clearWatchTopics() {
    const watchTopic = byId('watchTopic');
    try {
      await postForm('/broker-console/topic', { topics: '' });
      if (watchTopic) {
        watchTopic.value = '';
      }
      const watchTopicCurrent = byId('watchTopicCurrent');
      if (watchTopicCurrent) {
        watchTopicCurrent.textContent = '-';
      }
      showMessage('Watch topics cleared.');
      await refreshBrokerData();
    } catch (error) {
      showMessage('Failed to clear watch topics: ' + error.message, true);
    }
  }

  async function savePublishTopics() {
    const listNode = byId('publishTopicList');
    if (!listNode) return;
    const publishTopics = normalizeTopicList(listNode.value);
    const publishDefaultTopic = byId('publishTopic').value.trim();
    try {
      await postForm('/config', { publishTopics, publishDefaultTopic });
      showMessage('Publish topics saved.');
      await refreshBrokerData();
    } catch (error) {
      showMessage('Failed to save publish topics: ' + error.message, true);
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

    const btnStartWatching = byId('btnStartWatching');
    if (btnStartWatching) {
      btnStartWatching.addEventListener('click', startWatchingTopics);
    }
    const btnClearWatch = byId('btnClearWatch');
    if (btnClearWatch) {
      btnClearWatch.addEventListener('click', clearWatchTopics);
    }
    const btnSavePublishTopics = byId('btnSavePublishTopics');
    if (btnSavePublishTopics) {
      btnSavePublishTopics.addEventListener('click', savePublishTopics);
    }
    const btnUsePublishList = byId('btnUsePublishList');
    if (btnUsePublishList) {
      btnUsePublishList.addEventListener('click', () => {
        const listNode = byId('publishTopicList');
        if (!listNode) return;
        const topics = normalizeTopicList(listNode.value).split(',').filter(Boolean);
        if (topics.length > 0) {
          byId('publishTopic').value = topics[0];
          showMessage('Publish topic selected from saved list.');
        } else {
          showMessage('No saved publish topics available.', true);
        }
      });
    }
    const btnSendMessage = byId('btnSendMessage');
    if (btnSendMessage) {
      btnSendMessage.addEventListener('click', publishMessage);
    }
    const sendWebBtn = byId('btnSendWebMessage');
    if (sendWebBtn) {
      sendWebBtn.addEventListener('click', sendWebMessage);
    }

    const btnCopyWatch = byId('btnCopyWatch');
    if (btnCopyWatch) btnCopyWatch.addEventListener('click', async () => {
      try {
        const watchNode = byId('watchTopicCurrent');
        const watchTopic = (watchNode && watchNode.textContent) || '';
        if (!watchTopic || watchTopic === '-') {
          showMessage('No watched topics to copy.', true);
          return;
        }
        await navigator.clipboard.writeText(watchTopic);
        showMessage('Watched topics copied.');
      } catch (error) {
        showMessage('Clipboard unavailable in this browser.', true);
      }
    });

    Array.from(document.querySelectorAll('.js-watch-topic')).forEach((button) => {
      button.addEventListener('click', () => {
        appendWatchTopic(button.dataset.topic || '');
      });
    });

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

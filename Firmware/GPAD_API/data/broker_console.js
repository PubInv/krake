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

  function renderDrakeTable(drakes) {
    const tbody = byId('drakeTableBody');

    if (!drakes || drakes.length === 0) {
      tbody.innerHTML = '<tr><td colspan="9">No tracked devices yet.</td></tr>';
      byId('drakeCount').textContent = '0';
      return;
    }

    const rows = drakes.map((drake) => {
      const online = !!drake.online;
      const topicActive = !!drake.topicParticipant;
      const rowClass = online ? 'row-online' : 'row-offline';
      const onlinePill = online
        ? '<span class="pill ok">ONLINE</span>'
        : '<span class="pill bad">OFFLINE</span>';
      const topicPill = topicActive
        ? '<span class="pill ok">ACTIVE</span>'
        : '<span class="pill bad">IDLE</span>';

      return [
        '<tr class="' + rowClass + '">',
        '<td>' + parsePrimaryId(drake.id) + '</td>',
        '<td><code>' + (drake.id || '-') + '</code></td>',
        '<td>' + onlinePill + '</td>',
        '<td>' + topicPill + '</td>',
        '<td>' + (Number.isFinite(drake.rssi) ? drake.rssi : '-') + '</td>',
        '<td><code>' + (drake.status || '-') + '</code></td>',
        '<td><code>' + (drake.lastTopic || '-') + '</code></td>',
        '<td>' + ageText(drake.secondsSinceStatus) + '</td>',
        '<td>' + ageText(drake.secondsSinceTopic) + '</td>',
        '</tr>'
      ].join('');
    });

    tbody.innerHTML = rows.join('');
    byId('drakeCount').textContent = String(drakes.length);
  }

  function updateWatchUi(watchedTopics) {
    const normalized = normalizeTopicList(watchedTopics || '');
    byId('watchTopic').value = normalized;
    byId('watchTopicCurrent').textContent = normalized || '-';
  }

  async function refreshBrokerData() {
    try {
      const response = await fetch('/broker-console/data', { cache: 'no-store' });
      if (!response.ok) {
        throw new Error('Unable to load broker data');
      }

      const data = await response.json();
      updateWatchUi(data.watchedTopics || data.watchedTopic || '');
      renderDrakeTable(Array.isArray(data.drakes) ? data.drakes : []);
    } catch (error) {
      showMessage('Broker data refresh failed: ' + error.message, true);
    }
  }

  function appendWatchTopic(topic) {
    const watchInput = byId('watchTopic');
    const current = normalizeTopicList(watchInput.value);
    const parts = current ? current.split(',') : [];

    if (!parts.includes(topic)) {
      parts.push(topic);
    }

    watchInput.value = parts.join(',');
  }

  async function startWatchingTopics() {
    const topics = normalizeTopicList(byId('watchTopic').value);
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
    try {
      await postForm('/broker-console/topic', { topics: '' });
      byId('watchTopic').value = '';
      byId('watchTopicCurrent').textContent = '-';
      showMessage('Watch topics cleared.');
      await refreshBrokerData();
    } catch (error) {
      showMessage('Failed to clear watch topics: ' + error.message, true);
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

  function setupTemplates() {
    byId('btnUseAlm').addEventListener('click', () => {
      const base = parsePrimaryId(byId('publishTopic').value.trim()).replace(/[^A-Za-z0-9]/g, '');
      byId('publishTopic').value = (base || 'AABBCCDDEEFF') + '_ALM';
    });

    byId('btnUseAck').addEventListener('click', () => {
      const base = parsePrimaryId(byId('publishTopic').value.trim()).replace(/[^A-Za-z0-9]/g, '');
      byId('publishTopic').value = (base || 'AABBCCDDEEFF') + '_ACK';
    });

    byId('btnAlarmTemplate').addEventListener('click', () => {
      const timestamp = new Date().toISOString().replace(/[-:TZ.]/g, '').slice(0, 14);
      byId('publishPayload').value = 'a3 ' + timestamp + ' Test alarm message';
    });

    byId('btnInfoTemplate').addEventListener('click', () => {
      const timestamp = new Date().toISOString().replace(/[-:TZ.]/g, '').slice(0, 14);
      byId('publishPayload').value = 'a0 ' + timestamp + ' Test info message';
    });
  }

  function setupUiActions() {
    const menuToggle = byId('menuToggle');
    if (menuToggle) {
      menuToggle.addEventListener('click', toggleMenu);
    }

    byId('btnStartWatching').addEventListener('click', startWatchingTopics);
    byId('btnClearWatch').addEventListener('click', clearWatchTopics);
    byId('btnSendMessage').addEventListener('click', publishMessage);

    byId('btnCopyWatch').addEventListener('click', async () => {
      try {
        const watchTopic = byId('watchTopicCurrent').textContent || '';
        if (!watchTopic || watchTopic === '-') {
          showMessage('No watched topic to copy.', true);
          return;
        }
        await navigator.clipboard.writeText(watchTopic);
        showMessage('Watched topic copied.');
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


(function () {
  const navItems = [
    ['Update Firmware', '/update'],
    ['GDT Track Record', '/GDT_TrackHistory.html'],
    ['Electrical Test History', '/Electrical_testHistory.html'],
    ['Factory Test / Developer Monitor', '/monitor'],
    ['MQTT Device Monitor', '/device-monitor'],
    ['Broker Console', '/broker-console'],
    ['PMD Web UI', '/PMD_GPAD_API'],
    ['User Manual', '/manual'],
    ['Settings', '/settings'],
    ['Home', '/index.html', 'menu-home']
  ];

  function byId(id) { return document.getElementById(id); }
  function escapeHtml(value) {
    return String(value ?? '').replace(/[&<>'"]/g, (ch) => ({
      '&': '&amp;', '<': '&lt;', '>': '&gt;', "'": '&#39;', '"': '&quot;'
    }[ch]));
  }
  function splitCsv(value) {
    return String(value || '').split(',').map((item) => item.trim()).filter(Boolean);
  }
  function unique(values) {
    return Array.from(new Set(values.filter(Boolean)));
  }
  function getPublishTopics(settings) {
    return unique([settings.publishDefaultTopic, settings.publishTopic, ...splitCsv(settings.publishTopics)]);
  }
  async function postForm(url, bodyObj = {}) {
    const response = await fetch(url, {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: new URLSearchParams(bodyObj)
    });
    const text = await response.text();
    if (!response.ok) throw new Error(text || ('HTTP ' + response.status));
    return text;
  }
  async function getJson(url) {
    const response = await fetch(url, { cache: 'no-store' });
    if (!response.ok) throw new Error(await response.text() || ('HTTP ' + response.status));
    return response.json();
  }
  function showMessage(message, isError = false, id = 'message') {
    const node = byId(id);
    if (!node) return;
    node.textContent = message;
    node.style.color = isError ? '#b00020' : '#146620';
  }
  function toggleMenu() {
    const menu = byId('sideMenu');
    if (menu) menu.classList.toggle('open');
  }
  function mountLayout(title) {
    const headerTarget = byId('appHeader');
    const navTarget = byId('sideMenu');
    if (headerTarget) {
      headerTarget.className = 'topbar';
      headerTarget.innerHTML = '<div class="brand"><img src="/favicon.png" alt="KRAKE icon" class="brand-icon"><span>' + escapeHtml(title || 'KRAKE') + '</span></div><button id="menuToggle" class="menu-toggle" aria-label="Open menu">☰</button>';
    }
    if (navTarget) {
      navTarget.className = 'side-menu';
      navTarget.innerHTML = navItems.map(([label, href, cls]) => '<a href="' + href + '"' + (cls ? ' class="' + cls + '"' : '') + '>' + escapeHtml(label) + '</a>').join('');
    }
    const menuToggle = byId('menuToggle');
    if (menuToggle) menuToggle.addEventListener('click', toggleMenu);
  }
  function setText(id, value, fallback = '-') {
    const node = byId(id);
    if (node) node.textContent = value || fallback;
  }

  window.KrakeUI = { byId, escapeHtml, splitCsv, unique, getPublishTopics, postForm, getJson, showMessage, toggleMenu, mountLayout, setText };
})();

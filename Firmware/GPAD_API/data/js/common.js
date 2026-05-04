(function () {
  const navSections = [
    {
      id: 'user',
      title: '👤 User',
      defaultOpen: true,
      minRole: 'user',
      items: [
        ['GDT Track Record', '/GDT_TrackHistory.html'],
        ['User Manual', '/manual']
      ]
    },
    {
      id: 'admin',
      title: '⚙️ Admin',
      defaultOpen: false,
      minRole: 'admin',
      items: [
        ['Settings', '/settings'],
        ['Firmware Update', '/update']
      ]
    },
    {
      id: 'developer',
      title: '🛠 Developer',
      defaultOpen: false,
      minRole: 'developer',
      items: [
        ['PMD Web UI', '/PMD_GPAD_API'],
        ['Factory Test / Developer Monitor', '/monitor'],
        ['MQTT Device Monitor', '/device-monitor'],
        ['Debug / Logs', '/debug-logs'],
        ['Electrical Test History', '/Electrical_testHistory.html']
      ]
    }
  ];

  const roleRank = { user: 0, admin: 1, developer: 2 };
  const state = { currentRole: 'user', developerUnlocked: false };
  const DEV_PASSWORD = 'krake-dev';

  function byId(id) { return document.getElementById(id); }
  function escapeHtml(value) { return String(value ?? '').replace(/[&<>'"]/g, (ch) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', "'": '&#39;', '"': '&quot;' }[ch])); }
  function splitCsv(value) { return String(value || '').split(',').map((item) => item.trim()).filter(Boolean); }
  function unique(values) { return Array.from(new Set(values.filter(Boolean))); }
  function getPublishTopics(settings) { return unique([settings.publishDefaultTopic, settings.publishTopic, ...splitCsv(settings.publishTopics)]); }
  function hasRole(required) { return roleRank[state.currentRole] >= roleRank[required]; }
  function persistState() { localStorage.setItem('krake_role', state.currentRole); localStorage.setItem('krake_dev_unlocked', state.developerUnlocked ? '1' : '0'); }
  function restoreState() {
    const savedRole = localStorage.getItem('krake_role');
    const unlocked = localStorage.getItem('krake_dev_unlocked') === '1';
    state.currentRole = roleRank[savedRole] != null ? savedRole : 'user';
    state.developerUnlocked = unlocked;
    if (state.developerUnlocked) state.currentRole = 'developer';
  }

  async function postForm(url, bodyObj = {}) {
    const response = await fetch(url, { method: 'POST', headers: { 'Content-Type': 'application/x-www-form-urlencoded' }, body: new URLSearchParams(bodyObj) });
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
  function toggleMenu() { const menu = byId('sideMenu'); if (menu) menu.classList.toggle('open'); }

  function renderNav(navTarget) {
    const sections = navSections.filter((section) => {
      if (section.id === 'developer') return state.developerUnlocked;
      return hasRole(section.minRole);
    });
    const sectionHtml = sections.map((section) => {
      const links = section.items.map(([label, href, cls]) => '<a href="' + href + '"' + (cls ? ' class="' + cls + '"' : '') + '>' + escapeHtml(label) + '</a>').join('');
      return '<details class="menu-section"' + (section.defaultOpen ? ' open' : '') + '><summary>' + escapeHtml(section.title) + '</summary><div class="menu-links">' + links + '</div></details>';
    }).join('');

    const unlockHtml = state.developerUnlocked
      ? '<button id="devLockBtn" class="menu-unlock">🔒 Lock Developer Tools</button><div id="devUnlockMsg" class="note"></div>'
      : '<button id="devUnlockBtn" class="menu-unlock">🔧 Unlock Developer Mode</button><div id="devUnlockPanel" class="menu-unlock-panel hidden"><input id="devPassword" type="password" class="text-input" placeholder="Enter developer password"><button id="devSubmit" class="action-btn" type="button">Unlock</button><div id="devUnlockMsg" class="note"></div></div>';

    navTarget.innerHTML = sectionHtml + unlockHtml + '<a class="menu-home" href="/index.html">Home</a>';

    const unlockBtn = byId('devUnlockBtn');
    if (unlockBtn) unlockBtn.addEventListener('click', () => byId('devUnlockPanel')?.classList.toggle('hidden'));
    const lockBtn = byId('devLockBtn');
    if (lockBtn) lockBtn.addEventListener('click', () => { state.developerUnlocked = false; state.currentRole = 'user'; persistState(); renderNav(navTarget); });
    const submitBtn = byId('devSubmit');
    if (submitBtn) submitBtn.addEventListener('click', () => {
      const input = byId('devPassword');
      if (!input) return;
      if (input.value === DEV_PASSWORD) {
        state.developerUnlocked = true; state.currentRole = 'developer'; persistState(); renderNav(navTarget);
      } else {
        showMessage('Incorrect developer password.', true, 'devUnlockMsg');
      }
    });
  }

  function mountLayout(title) {
    restoreState();
    const headerTarget = byId('appHeader');
    const navTarget = byId('sideMenu');
    if (headerTarget) {
      headerTarget.className = 'topbar';
      headerTarget.innerHTML = '<a href="/index.html" class="brand" aria-label="Go to Home"><img src="/favicon.png" alt="KRAKE icon" class="brand-icon"><span>' + escapeHtml(title || 'KRAKE') + '</span></a><button id="menuToggle" class="menu-toggle" aria-label="Open menu">☰</button>';
    }
    if (navTarget) { navTarget.className = 'side-menu'; renderNav(navTarget); }
    const menuToggle = byId('menuToggle');
    if (menuToggle) menuToggle.addEventListener('click', toggleMenu);
  }
  function setText(id, value, fallback = '-') { const node = byId(id); if (node) node.textContent = value || fallback; }

  window.KrakeUI = { byId, escapeHtml, splitCsv, unique, getPublishTopics, postForm, getJson, showMessage, toggleMenu, mountLayout, setText };
})();

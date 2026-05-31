(function () {
  const mqttBroker = Object.freeze({
    hostname: 'krakepubinv.cloud.shiftr.io',
    wssUrl: 'wss://krakepubinv.cloud.shiftr.io',
    embedUrl: 'https://krakepubinv.cloud.shiftr.io/embed?widgets=1',
    username: 'krakepubinv',
    profiles: [{ id: 'krake', label: 'Krake PubInv', wssUrl: 'wss://krakepubinv.cloud.shiftr.io', username: 'krakepubinv' }, { id: 'custom', label: 'Custom broker' }]
  });
  const navSections = [
    {
      id: 'user',
      title: '👤 User',
      defaultOpen: true,
      minRole: 'user',
      items: [
        ['GDT Track Record', '/GDT_TrackHistory.html'],
        ['User Manual', '/manual.html']
      ]
    },
    {
      id: 'admin',
      title: '⚙️ Admin',
      defaultOpen: false,
      minRole: 'admin',
      items: [
        ['Settings', '/settings.html'],
        ['Firmware Update', '/update']
      ]
    },
    {
      id: 'developer',
      title: '🛠 Developer',
      defaultOpen: false,
      minRole: 'developer',
      items: [
        ['PMD Web UI', '/PMD_GPAD_API.html'],
        ['Factory Test / Developer Monitor', '/monitor.html'],
        ['MQTT Device Monitor', '/device-monitor.html'],
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

  function mountFooter() {
    let footer = document.querySelector('footer.footer');
    if (!footer) {
      footer = document.createElement('footer');
      footer.className = 'footer';
      document.body.appendChild(footer);
    }

    let links = footer.querySelector('.footer-links');
    if (!links) {
      links = document.createElement('div');
      links.className = 'footer-links';
      const message = footer.querySelector('#message');
      footer.insertBefore(links, message || footer.firstChild);
    }

    if (!links.querySelector('[data-footer-link="pubinv"]')) {
      const websiteLink = footer.querySelector('a[href="https://pubinv.org"]') || document.createElement('a');
      websiteLink.href = 'https://pubinv.org';
      websiteLink.target = '_blank';
      websiteLink.rel = 'noopener noreferrer';
      websiteLink.dataset.footerLink = 'pubinv';
      websiteLink.textContent = 'pubinv.org';
      links.appendChild(websiteLink);
    }

    if (!links.querySelector('[data-footer-link="github"]')) {
      const githubLink = document.createElement('a');
      githubLink.href = 'https://github.com/PubInv/krake';
      githubLink.target = '_blank';
      githubLink.rel = 'noopener noreferrer';
      githubLink.className = 'github-link';
      githubLink.dataset.footerLink = 'github';
      githubLink.setAttribute('aria-label', 'View KRAKE source code on GitHub');
      githubLink.title = 'View KRAKE on GitHub';
      githubLink.innerHTML = '<svg class="github-icon" viewBox="0 0 24 24" aria-hidden="true" focusable="false"><path fill="currentColor" d="M12 .7a11.3 11.3 0 0 0-3.6 22c.6.1.8-.2.8-.5v-2.1c-3.3.7-4-1.4-4-1.4-.5-1.4-1.3-1.7-1.3-1.7-1.1-.8.1-.8.1-.8 1.2.1 1.8 1.2 1.8 1.2 1.1 1.8 2.8 1.3 3.5 1 .1-.8.4-1.3.8-1.6-2.7-.3-5.5-1.3-5.5-5.9 0-1.3.5-2.4 1.2-3.2-.1-.3-.5-1.6.1-3.2 0 0 1-.3 3.3 1.2a11.5 11.5 0 0 1 6 0c2.3-1.5 3.3-1.2 3.3-1.2.6 1.6.2 2.9.1 3.2.8.9 1.2 1.9 1.2 3.2 0 4.6-2.8 5.6-5.5 5.9.4.4.8 1.1.8 2.2v3.2c0 .3.2.6.8.5A11.3 11.3 0 0 0 12 .7Z"/></svg><span class="visually-hidden">GitHub</span>';
      links.appendChild(githubLink);
    }
  }

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

    navTarget.innerHTML = sectionHtml + unlockHtml + '<a class="menu-home" href="/">Home</a>';

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
      headerTarget.innerHTML = '<div class="brand"><a href="/" aria-label="Go to Home"><img src="/favicon.png" alt="KRAKE icon" class="brand-icon"></a><span>' + escapeHtml(title || 'KRAKE') + '</span></div><button id="menuToggle" class="menu-toggle" aria-label="Open menu">☰</button>';
    }
    if (navTarget) { navTarget.className = 'side-menu'; renderNav(navTarget); }
    mountFooter();
    const menuToggle = byId('menuToggle');
    if (menuToggle) menuToggle.addEventListener('click', toggleMenu);
  }
  function setText(id, value, fallback = '-') { const node = byId(id); if (node) node.textContent = value || fallback; }

  window.KrakeUI = { mqttBroker, byId, escapeHtml, splitCsv, unique, getPublishTopics, postForm, getJson, showMessage, toggleMenu, mountLayout, setText };
})();

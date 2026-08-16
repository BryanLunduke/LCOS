(() => {
  'use strict';

  const $ = (selector, root = document) => root.querySelector(selector);
  const $$ = (selector, root = document) => [...root.querySelectorAll(selector)];

  const boot = $('#boot');
  const desktop = $('#desktop');
  const bootLog = $('#bootLog');
  const skipBoot = $('#skipBoot');
  const appsButton = $('#applicationsButton');
  const appsMenu = $('#applicationsMenu');
  const taskButtons = $('#taskButtons');
  const toast = $('#toast');
  const clock = $('#clock');

  let zCounter = 500;
  let toastTimer;
  let updateCheckTimer;
  let bootFinished = false;

  const bootLines = [
    '[    0.000000] LCOS BIOS: locating opinions... found 47',
    '[    0.041337] Loading kernel: vibes-6.9.420-lcos',
    '[    0.071000] Detecting hardware: beige tower preferred, modern hardware tolerated',
    '[    0.112358] Starting meritocracy-daemon.service                 [ OK ]',
    '[    0.161803] Mounting /home                                     [ OK ]',
    '[    0.200000] Mounting /dev/null for telemetry                  [ OK ]',
    '[    0.271828] Negotiating X11/Wayland peace treaty              [ FAILED ]',
    '[    0.314159] Falling back to strongly held opinions            [ OK ]',
    '[    0.404000] Checking swear jar filesystem                     [ CLEAN ]',
    '[    0.512000] Initializing package format .debate               [ OK ]',
    '[    0.640000] Starting desktop environment: LCDE',
    '[    0.777777] WARNING: this is a website, not an operating system.',
    '[    1.000000] Welcome to LCOS. Please keep all discourse inside the viewport.'
  ];

  function showToast(message, duration = 3200) {
    clearTimeout(toastTimer);
    toast.textContent = message;
    toast.hidden = false;
    toastTimer = setTimeout(() => { toast.hidden = true; }, duration);
  }

  function finishBoot() {
    if (bootFinished) return;
    bootFinished = true;
    boot.hidden = true;
    desktop.hidden = false;
    openWindow('aboutWindow');
    updateClock();
    setInterval(updateClock, 1000);
  }

  async function runBoot() {
    const reduceMotion = matchMedia('(prefers-reduced-motion: reduce)').matches;
    if (reduceMotion) {
      bootLog.textContent = bootLines.join('\n');
      finishBoot();
      return;
    }

    for (const line of bootLines) {
      if (bootFinished) break;
      bootLog.textContent += `${line}\n`;
      bootLog.scrollTop = bootLog.scrollHeight;
      await new Promise(resolve => setTimeout(resolve, 125 + Math.random() * 110));
    }
    await new Promise(resolve => setTimeout(resolve, 380));
    finishBoot();
  }

  function updateClock() {
    const now = new Date();
    clock.textContent = now.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
    clock.title = now.toLocaleString();
  }

  function titleFor(win) {
    return $('.window-title', win)?.textContent?.trim() || win.id;
  }

  function ensureTaskButton(win) {
    let button = taskButtons.querySelector(`[data-task-for="${win.id}"]`);
    if (!button) {
      button = document.createElement('button');
      button.className = 'task-button';
      button.type = 'button';
      button.dataset.taskFor = win.id;
      button.textContent = titleFor(win);
      button.addEventListener('click', () => {
        if (win.hidden) openWindow(win.id);
        else if (win.classList.contains('active-window')) minimizeWindow(win);
        else focusWindow(win);
      });
      taskButtons.append(button);
    }
    button.classList.toggle('active', !win.hidden && win.classList.contains('active-window'));
  }

  function syncTaskButtons() {
    $$('.window').forEach(win => {
      const button = taskButtons.querySelector(`[data-task-for="${win.id}"]`);
      if (!button) return;
      button.classList.toggle('active', !win.hidden && win.classList.contains('active-window'));
    });
  }

  function focusWindow(win) {
    if (!win || win.hidden) return;
    $$('.window').forEach(item => item.classList.remove('active-window'));
    win.classList.add('active-window');
    win.style.zIndex = String(++zCounter);
    ensureTaskButton(win);
    syncTaskButtons();
  }

  function focusOpenedWindow(win) {
    if (!win.hasAttribute('tabindex')) win.tabIndex = -1;
    const target = win.id === 'terminalWindow'
      ? $('#terminalInput', win)
      : $('.window-body input, .window-body button, .window-body a[href]', win) || win;
    setTimeout(() => target?.focus(), 0);
  }

  function openWindow(id) {
    const win = document.getElementById(id);
    if (!win) return;
    win.hidden = false;
    ensureTaskButton(win);
    focusWindow(win);
    appsMenu.hidden = true;
    appsButton.setAttribute('aria-expanded', 'false');
    focusOpenedWindow(win);
  }

  function closeWindow(win) {
    win.hidden = true;
    win.classList.remove('active-window');
    const button = taskButtons.querySelector(`[data-task-for="${win.id}"]`);
    button?.remove();
    const visible = $$('.window').filter(item => !item.hidden);
    if (visible.length) focusWindow(visible.sort((a, b) => (+b.style.zIndex || 0) - (+a.style.zIndex || 0))[0]);
  }

  function minimizeWindow(win) {
    win.hidden = true;
    win.classList.remove('active-window');
    ensureTaskButton(win);
    syncTaskButtons();
  }

  function toggleMaximize(win) {
    win.classList.toggle('maximized');
    focusWindow(win);
  }

  function toggleApplicationsMenu() {
    appsMenu.hidden = !appsMenu.hidden;
    appsButton.setAttribute('aria-expanded', String(!appsMenu.hidden));
  }

  function makeDraggable(win) {
    const titlebar = $('.window-titlebar', win);
    if (!titlebar) return;

    let drag = null;

    titlebar.addEventListener('pointerdown', event => {
      if (event.target.closest('button') || win.classList.contains('maximized') || innerWidth <= 900) return;
      focusWindow(win);
      const rect = win.getBoundingClientRect();
      drag = { x: event.clientX - rect.left, y: event.clientY - rect.top };
      titlebar.setPointerCapture(event.pointerId);
      event.preventDefault();
    });

    titlebar.addEventListener('pointermove', event => {
      if (!drag) return;
      const maxX = Math.max(0, innerWidth - win.offsetWidth);
      const maxY = Math.max(42, innerHeight - win.offsetHeight - 48);
      const x = Math.min(maxX, Math.max(0, event.clientX - drag.x));
      const y = Math.min(maxY, Math.max(42, event.clientY - drag.y));
      win.style.left = `${x}px`;
      win.style.top = `${y}px`;
      win.style.setProperty('--x', `${x}px`);
      win.style.setProperty('--y', `${y}px`);
    });

    const endDrag = event => {
      if (!drag) return;
      drag = null;
      try { titlebar.releasePointerCapture(event.pointerId); } catch (_) { /* already released */ }
    };

    titlebar.addEventListener('pointerup', endDrag);
    titlebar.addEventListener('pointercancel', endDrag);
    titlebar.addEventListener('dblclick', event => {
      if (!event.target.closest('button')) toggleMaximize(win);
    });
  }

  function panicAboutSystemd() {
    appsMenu.hidden = true;
    appsButton.setAttribute('aria-expanded', 'false');
    showToast('SYSTEMD DETECTED: beginning a calm, proportionate 47-minute panel discussion.', 5200);
    openWindow('terminalWindow');
    appendTerminal('root@lcos:~# systemctl enable common-sense.target', 'term-bad');
    appendTerminal('Failed: dependency "internet-consensus.service" does not exist.', 'term-gold');
  }

  function appendTerminal(text, className = '') {
    const output = $('#terminalOutput');
    const line = document.createElement('div');
    if (className) line.className = className;
    line.textContent = text;
    output.append(line);
    output.scrollTop = output.scrollHeight;
  }

  const terminalCommands = {
    help() {
      appendTerminal('Available commands: help, about, uname -a, neofetch, ls, apt update, sudo, systemctl, wayland, x11, ethics, clear, reboot, exit', 'term-dim');
    },
    about() {
      appendTerminal('LCOS Browser Visualization Edition — a satire fork. No ISO. No installer. No warranty. Considerable confidence.');
    },
    'uname -a'() {
      appendTerminal('Linux lcos 6.9.420-lcos #1 SMP PREEMPT_DYNAMIC SatireOS x86_64 GNU/Linux');
    },
    neofetch() {
      appendTerminal('        LLLLL       trent@lcos');
      appendTerminal('        L           ---------');
      appendTerminal('        L           OS: LCOS Browser Edition');
      appendTerminal('        L           Kernel: vibes-6.9.420');
      appendTerminal('        LLLLLLL     Shell: bash-with-opinions');
      appendTerminal('                    DE: LCDE');
      appendTerminal('                    Packages: 5 (.debate)');
      appendTerminal('                    Uptime: longer than expected');
    },
    ls() {
      appendTerminal('Desktop  Documents  screenshots  opinions  README.md  absolutely-not-an-iso.iso', 'term-good');
    },
    'apt update'() {
      appendTerminal('Hit:1 https://mirror.invalid stable InRelease');
      appendTerminal('Get:2 discourse://localhost opinions [47 MB]');
      appendTerminal('Reading package lists... Done');
      appendTerminal('3 packages can be upgraded. 38 arguments can be restarted.', 'term-gold');
    },
    sudo() {
      appendTerminal('trent is not in the sudoers file. This incident will be discussed on a podcast.', 'term-bad');
    },
    systemctl() {
      appendTerminal('systemd-opinion-generator.service is active (running) despite multiple objections.', 'term-gold');
    },
    wayland() {
      appendTerminal('wayland: command acknowledged. Desktop immediately split into two mailing lists.', 'term-bad');
    },
    x11() {
      appendTerminal('X.Org Server 21.1.∞ — if it was good enough for 1987, it can probably open xterm.', 'term-good');
    },
    ethics() {
      openWindow('ethicsWindow');
      appendTerminal('Ethics daemon opened. Please remain excellent.', 'term-good');
    },
    clear() {
      $('#terminalOutput').replaceChildren();
    },
    reboot() {
      appendTerminal('Reboot denied: this is JavaScript and we are trying to maintain standards.', 'term-bad');
    },
    exit() {
      minimizeWindow($('#terminalWindow'));
    }
  };

  function executeTerminal(raw) {
    const command = raw.trim();
    if (!command) return;
    appendTerminal(`trent@lcos:~$ ${command}`);
    const normalized = command.toLowerCase().replace(/\s+/g, ' ');

    if (terminalCommands[normalized]) {
      terminalCommands[normalized]();
      return;
    }

    if (normalized.startsWith('sudo ')) {
      terminalCommands.sudo();
      return;
    }

    if (normalized.startsWith('systemctl ')) {
      terminalCommands.systemctl();
      return;
    }

    appendTerminal(`bash: ${command}: command not found. Have you considered filing an issue about it?`, 'term-bad');
  }

  function initTerminal() {
    const form = $('#terminalForm');
    const input = $('#terminalInput');
    appendTerminal('LCOS terminal 0.1. Type "help". Please do not attempt actual administration.', 'term-gold');
    form.addEventListener('submit', event => {
      event.preventDefault();
      executeTerminal(input.value);
      input.value = '';
    });
  }

  function initPackageManager() {
    const log = $('#updateLog');
    const summary = $('#updateSummary');

    $('#checkUpdates').addEventListener('click', () => {
      clearTimeout(updateCheckTimer);
      summary.textContent = 'Checking mirrors and prevailing sentiment…';
      log.textContent = 'Contacting mirror.invalid...\nReading package lists...\nCross-referencing comments...\nIgnoring one guy who suggested Electron...\n\n3 updates found; 1 package held due to ideological conflict.';
      updateCheckTimer = setTimeout(() => {
        updateCheckTimer = null;
        summary.textContent = '3 updates available; 1 eternal argument held back';
        showToast('Updates checked. Your computer is now 12% more aware of discourse.');
      }, 650);
    });

    $('#applyUpdates').addEventListener('click', () => {
      clearTimeout(updateCheckTimer);
      updateCheckTimer = null;
      log.textContent = 'Preparing upgrade...\nInstalling no-cursin-filter 1.5... OK\nInstalling lcos-base 0.1.1... OK\nRestarting meritocracy-daemon... OK\nHolding systemd-opinion-generator... obviously\n\nDone. Reboot strongly discouraged because this is a webpage.';
      summary.textContent = 'System is completely up to date, spiritually';
      showToast('Upgrade complete. Zero real packages were modified.');
    });
  }

  function initEthics() {
    const input = $('#ethicsInput');
    const result = $('#ethicsResult');
    const forbidden = /\b(fuck|shit|cunt|bastard|damn|bloody hell)\b/i;

    $('#ethicsCheck').addEventListener('click', () => {
      const value = input.value.trim();
      if (!value) {
        result.textContent = 'Nothing entered. Ethics daemon awards points for restraint.';
      } else if (forbidden.test(value)) {
        result.textContent = '⚠ CURSIN\' DETECTED. One virtual coin deposited in the kernel swear jar.';
      } else {
        result.textContent = '✓ Excellent. Meritocracy daemon reluctantly approves this sentence.';
      }
    });

    input.addEventListener('keydown', event => {
      if (event.key === 'Enter') $('#ethicsCheck').click();
    });
  }

  appsButton.addEventListener('click', event => {
    event.stopPropagation();
    toggleApplicationsMenu();
  });

  document.addEventListener('click', event => {
    if (!event.target.closest('#applicationsMenu') && !event.target.closest('#applicationsButton')) {
      appsMenu.hidden = true;
      appsButton.setAttribute('aria-expanded', 'false');
    }
  });

  $$('[data-open]').forEach(button => {
    button.addEventListener('click', () => openWindow(button.dataset.open));
  });

  $$('[data-action="panic"]').forEach(button => button.addEventListener('click', panicAboutSystemd));

  $$('.window').forEach(win => {
    makeDraggable(win);
    win.addEventListener('pointerdown', () => focusWindow(win));

    const closeButton = $('[data-close]', win);
    const minimizeButton = $('[data-minimize]', win);
    const maximizeButton = $('[data-maximize]', win);

    closeButton?.setAttribute('aria-label', 'Close');
    minimizeButton?.setAttribute('aria-label', 'Minimize');
    maximizeButton?.setAttribute('aria-label', 'Maximize');

    closeButton?.addEventListener('click', () => closeWindow(win));
    minimizeButton?.addEventListener('click', () => minimizeWindow(win));
    maximizeButton?.addEventListener('click', () => toggleMaximize(win));
  });

  $('#showDesktop').addEventListener('click', () => {
    const visible = $$('.window').filter(win => !win.hidden);
    if (visible.length) {
      visible.forEach(minimizeWindow);
      showToast('Desktop shown. Productivity has been minimized.');
    } else {
      openWindow('aboutWindow');
    }
  });

  clock.addEventListener('click', () => showToast(`Current LCOS time: ${new Date().toLocaleString()}. Still no RTC driver drama.`));
  skipBoot.addEventListener('click', finishBoot);

  window.addEventListener('keydown', event => {
    if (event.key === 'Escape') {
      appsMenu.hidden = true;
      appsButton.setAttribute('aria-expanded', 'false');
    }
  });

  initTerminal();
  initPackageManager();
  initEthics();
  runBoot();
})();

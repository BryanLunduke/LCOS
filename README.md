# LCOS — The Operating System We Absolutely Did Not Build

> **Satire fork / browser visualization. This is not a Linux distribution. There is no ISO. Please do not flash `index.html` to a USB stick.**

LCOS is an interactive GitHub Pages mock desktop imagining what the Lunduke Computer Operating System could look like if a README, some screenshots, Linux discourse, and far too much confidence achieved sentience.

## Live site

https://qsolkcb.github.io/LCOS/

## Included

- ceremonial fake boot sequence
- LCDE browser desktop with draggable, minimizable and maximizable windows
- fake terminal with `neofetch`, `uname -a`, `apt update`, `systemctl`, `x11`, `wayland`, `ethics`, and more
- `.debate` package manager
- parody Issue Control Center
- Ethics Daemon based on the existing `CodeOfEthics.md`
- gallery preserving all existing LCOS screenshots
- GitHub Pages deployment and static-site CI
- no frameworks, telemetry, installer, kernel, init system, or actual distro

## Run locally

```bash
python3 -m http.server 8000
```

Then open `http://localhost:8000`.

## Upstream and satire note
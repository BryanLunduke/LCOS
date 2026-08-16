# LCOS — The Operating System We Absolutely Did Not Build

> **Satire fork / browser visualization. This is not a Linux distribution. There is no ISO. Please do not flash `index.html` to a USB stick.**

This fork turns the idea of **The Lunduke Computer Operating System** into an interactive GitHub Pages desktop: part mock operating system, part Linux-community archaeology, part extremely unnecessary package-manager UI.

## Live site

**https://qsolkcb.github.io/LCOS/**

If GitHub Pages has just been enabled, allow GitHub to publish the latest branch before assuming the kernel has panicked.

## What is here

- A ceremonial fake boot sequence with a `vibes-6.9.420-lcos` kernel.
- **LCDE**, the Lunduke Computer Desktop Environment, implemented as plain HTML/CSS/JavaScript.
- Draggable, minimizable and maximizable desktop windows.
- A fake terminal with commands including `neofetch`, `uname -a`, `apt update`, `systemctl`, `x11`, `wayland`, and `ethics`.
- A package manager whose native package format is naturally `.debate`.
- An Issue Control Center for visualizing the traditional Linux development process: three lines of code and forty-seven opinions.
- An Ethics Daemon based on the repository's existing `CodeOfEthics.md` rules.
- A screenshot gallery preserving the existing LCOS images in `screenshots/`.
- No frameworks, build step, package registry, analytics, telemetry, cookies, installers, kernels, init systems, or legitimate reasons for this to be as elaborate as it is.

## Run locally

Because the site is static, either open `index.html` directly or serve the directory:

```bash
python3 -m http.server 8000
```

Then visit `http://localhost:8000`.

## Upstream

The actual project this satire fork descends from is:

-
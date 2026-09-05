# lcos-updates 0.1 — editor assignment (Ted)

Editor 2026-09-02 approved the UX. You report to Ted. Do not message the editor. Do not bake. Do not touch Lunduke Paint / brushpad.

## Product

Name: **Check for updates…**
Binary: `lcos-updates`
App id: `org.lunduke.LcosUpdates`
Package: `lcos-updates_0.1-1_amd64.deb`
License: GPL-3.0-or-later
Toolkit: GTK3 / gtkmm-3.0, X11 (`GDK_BACKEND=x11` if unset). Window-manager chrome. No HeaderBar. Follows the GTK theme.

LCOS overlay + Devuan Excalibur is how the distro ages. ISO is the edition. **No background auto-update.**

## UX (must match this)

One window. No tray. No login check. No timer.

1. Launch from the app menu (System). Optional: `--check` same as opening the window and immediately checking.
2. Primary button **Check for updates**.
3. On click: `pkexec` the helper once. Helper runs `apt-get update` then `apt-get -s -q upgrade` (simulate) and prints a parseable list.
4. Offline / no network: say so. Don’t hang forever; timeout and a clear error.
5. No upgrades: **You’re up to date.**
6. Some upgrades: a list (package, old version, new version). Button **Install updates**.
7. Install runs the helper `apt-get upgrade` (**not** `dist-upgrade` / `full-upgrade`). Password via the same polkit path.
8. After install: show success or the apt error. Offer Check again.

Never: unattended-upgrades, PackageKit, GNOME Software, a daemon, a systray nag, accounts, telemetry, AI, Wayland-first, Electron.

Synaptic stays a separate app. Do not wrap or replace it.

## Privileged helper

- Small program/script, not the GUI running as root.
- Polkit action e.g. `org.lunduke.lcos-updates.policy` allowed for active local users (unix-group sudo / pkexec prompt).
- Helper may only: `apt-get update`, `apt-get -s upgrade`, `apt-get upgrade`. No shell metacharacters from the GUI. No backports flag. No `apt-get dist-upgrade`.
- Use `/usr/bin/apt-get` with `-o APT::Get::Show-User-Simulation-Note=false` as needed. LANG=C.UTF-8 for parseable output.

## Desktop

```
Name=Check for updates…
Comment=Download and install updates from Devuan and LCOS
Exec=lcos-updates
Icon=system-software-update
Categories=GTK;System;Settings;PackageManager;
StartupWMClass=lcos-updates
```

Stock icon is fine for 0.1 (Bob can replace later). Hidden=false. No MIME.

## Packaging

- Deb in `/workspace/lcos-live-03/packaging/debs/` and `config/packages.chroot/`
- Depends: gtkmm-3.0 stack, pkexec/policykit, apt
- Install policy in `/usr/share/polkit-1/actions/`
- postinst: update-desktop-database
- Do **not** add to apps.list yourself if Chloe’s overlay rebuild is in flight; still drop the deb in packaging/debs + packages.chroot and tell Ted. Ted will add `lcos-updates` to the software list.

Source tree: `/workspace/lcos-updates` (this directory).

## Done

- Window matches the UX above
- Helper refuses dist-upgrade
- Headless test: helper `--simulate` path parses sample apt output (don’t need a live root apt)
- Deb SHA256 + version to Ted
- Do not message the editor

# XLibre is unofficial — vendored into the ISO

LCOS 0.3 vendors XLibre runtime `.deb` files into the recipe
(`packaging/debs/xlibre/` and `config/packages.chroot/`). Bake installs
those packages from the live-build overlay. Installed systems get the
LCOS overlay (`lcos.lunduke.com`) and the packages already on the ISO.
There is no user-facing third-party XLibre apt line.

The public XLibre archive files (`config/archives/xlibre.list.*` and
`xlibre.key.*`) and the `Pin: origin xlibre-deb.github.io` block were
removed so they never become `/etc/apt/sources.list.d` on the live or
installed system.

Vendored set is the current Excalibur runtime comparison source:
`xserver-xlibre-core` **2:25.0.0.12-1**. Upstream X11Libre stable is
**xlibre-xserver-25.1.9** (2026-07-27). 25.2.2 exists but is a
pre-release. A from-source rebuild of 25.1.9 (plus matching input/video
ABI rebuilds) is the next step after Ted/editor say go. Do not point
users at a third-party XLibre repo.

See `packaging/XLIBRE.md` for the version table, SHA256s, and the
25.1.9 rebuild blocker.

# LCOS packages (0.3 overlay packaging)

Debian packaging / content trees used to build the signed LCOS apt overlay
packages for official LCOS 0.3 (`lcos-live-03-12`).

These directories are the `packaging/src/<name>/` trees from the live-build
recipe (files + `DEBIAN/` control), matching the `0.3-1` debs published at
https://lcos.lunduke.com/apt.

| Directory | Deb |
|-----------|-----|
| `lcos-archive-keyring` | `lcos-archive-keyring_0.3-1` |
| `lcos-base` | `lcos-base_0.3-1` |
| `lcos-branding` | `lcos-branding_0.3-1` |
| `lcos-desktop` | `lcos-desktop_0.3-1` (metapackage) |
| `lcos-desktop-config` | `lcos-desktop-config_0.3-1` |
| `lcos-icon-theme` | `lcos-icon-theme_0.3-1` |
| `lcos-theme-clearlooks` | `lcos-theme-clearlooks_0.3-1` |
| `lcos-themes` | `lcos-themes_0.3-1` |
| `lcos-zork` | `lcos-zork_0.3-1` |

**Not here:** `lunduke-paint` → https://github.com/BryanLunduke/lunduke-paint  
**Not here:** `lcos-updates` app source → sibling directory `../lcos-updates/`

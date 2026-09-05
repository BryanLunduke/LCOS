# XLibre from-source rebuild (LCOS 0.3)

Rebuilt 2026-08-25 (America/New_York) for this 0.3 recipe only.
**Not** added to the already-signed `packaging/apt-repo/` (that would
invalidate InRelease / Release.gpg). **Not** a user-facing apt source.
Bake installs these from `config/packages.chroot/`; the same bytes live
in `packaging/debs/xlibre/`.

Rollback of the previous vendored 25.0.0.12 / 7.8+4 set is under
`packaging/debs/xlibre-25.0.0.12-vendor/`.

No ISO was baked. Nothing was uploaded. `xlibre-deb.github.io` was not
added. LCOS-Branding was not used.

## Build environment

| Item | Value |
|------|-------|
| Method | Devuan Excalibur amd64 **chroot** (not a Trixie host build) |
| Bootstrap | `debootstrap --variant=minbase` using helper `lcos-live-03/helpers/debootstrap-excalibur` |
| Mirror | `http://ftp.fau.de/devuan/merged` |
| Keyring | `/usr/share/keyrings/devuan-archive-keyring.gpg` (devuan-keyring 2026.01.13) |
| Chroot libc | `libc6` 2.41-12+deb13u3 (Excalibur). Depends use `libc6 (>= 2.38)` or shlibdeps; nothing newer than Excalibur. |
| Toolchain | gcc 14.2.0, meson 1.7.0, ninja 1.12.1 |
| Maintainer | LCOS <lcos@lunduke.com> |
| Signing | **UNSIGNED** (`dpkg-deb --build`) |
| Server meson | `--buildtype=release`, `-Dxorg=true`, Xephyr/Xnest/Xvfb/xfbdev off, `-Dsystemd_logind=false`, `-Dseatd_libseat=true`, `-Dlegacy_nvidia_padding=false` |

Work lived under `/workspace/lcos-live-03` and `/tmp` (chroot `/tmp/excalibur-xlibre`, sources `/tmp/xlibre-build`). `/workspace/lcos-live` (0.2) was not touched.

## ABI (from xlibre-xserver-25.1.9)

Read after `ninja install` from `xorg-server.pc` and
`hw/xfree86/common/xf86Module.h`.

| Variable | Value | Notes |
|----------|-------|--------|
| `abi_xinput` | **26.0** | pkg-config + header `ABI_XINPUT_VERSION` |
| `abi_videodrv` (active) | **28.0** | `legacy_nvidia_padding=false` → `SET_ABI_VERSION(28, 0)` |
| pkg-config `abi_videodrv` raw | `28.128.0` | meson awk matches both `#ifdef` lines in `xf86Module.h` and concatenates `28.1`+`28.0`. **Ignored.** Provides use major **28**. |
| module dir | `/usr/lib/xorg/modules/xlibre-25` | 25.1 ABI tag (was `xlibre-25.0` on 25.0.0.12) |

`xserver-xlibre-core` Provides: `xorg-input-abi-26`, `xorg-video-abi-28`,
`xorg-video-abi-25` (NVIDIA compat, same as the old 25.0.0.12 package),
`xserver-xorg-core (= 2:21.98)`, `xserver-xorg-video-modesetting`.

Drivers Depend on those ABI virtuals plus `xserver-xlibre-core (>= 2:25.1.9)`.

## Git tags (shallow clone `--depth 1 --branch TAG`, not master)

| Repo | Tag | Commit |
|------|-----|--------|
| [X11Libre/xserver](https://github.com/X11Libre/xserver) | **xlibre-xserver-25.1.9** | `f844d7a86c10fd5cc89083e7daf97b862b8ca1fd` |
| [X11Libre/xf86-input-libinput](https://github.com/X11Libre/xf86-input-libinput) | xlibre-xf86-input-libinput-25.0.1 | `4eb6691efeef4969a05b1d6b77d980943fb9760e` |
| [X11Libre/xf86-video-amdgpu](https://github.com/X11Libre/xf86-video-amdgpu) | xlibre-xf86-video-amdgpu-25.1.2 | `8b491b5a07064d9d8f849c51c9dd8354706a2337` |
| [X11Libre/xf86-video-ati](https://github.com/X11Libre/xf86-video-ati) | xlibre-xf86-video-ati-25.0.1 | `e192d6a87495e3b91a1c85c3a2243def23abecd7` |
| [X11Libre/xf86-video-fbdev](https://github.com/X11Libre/xf86-video-fbdev) | xlibre-xf86-video-fbdev-25.0.0 | `b69995d4da347283400d9cc185eeda041288c9da` |
| [X11Libre/xf86-video-nouveau](https://github.com/X11Libre/xf86-video-nouveau) | xlibre-xf86-video-nouveau-25.0.1 | `6cfa57f1eeb19a9a09f0aaea49d5cf24a37905ee` |
| [X11Libre/xf86-video-vesa](https://github.com/X11Libre/xf86-video-vesa) | xlibre-xf86-video-vesa-25.0.0 | `850ec6dc643bf18d9011770b5b6fef7edfb927ce` |
| [X11Libre/xf86-video-vmware](https://github.com/X11Libre/xf86-video-vmware) | xlibre-xf86-video-vmware-25.0.0 | `3580a57072304cecdf256733acfba1dc11cd366d` |

No `xf86-video-radeon` repo (radeon lives in ati). No invented tags.
No intel/qxl/legacy tail. 25.2.2 was not used.

## Package table

| Package | Version | Tag | ABI | Bytes | SHA256 | Status |
|---------|---------|-----|-----|------:|--------|--------|
| xserver-xlibre-core | 2:25.1.9-1+lcos1 | xlibre-xserver-25.1.9 | provides input-26 / video-28 | 1553512 | e34b86d703cf851988f799f9bd125ad98a48a5ff28bdfe459010806cbb2f3821 | ok |
| xserver-xlibre-common | 2:25.1.9-1+lcos1 | xlibre-xserver-25.1.9 | n/a | 16488 | 476bb5fcc91db89958811388c7c0992fb0b49b8110b19b4eecad14e5b5f2ad0a | ok |
| xserver-xlibre | 1:25.1.9-1+lcos1 | metapackage | n/a | 1380 | a460c0c5ace3e037ebf3edff1717c1fbcfdaaeb198edb37ad3403fb2c7217760 | ok |
| xlibre | 1:25.1.9-1+lcos1 | metapackage | n/a | 1476 | e0d8ddda1cda7493aff3ef736176238b747469890cb9b8dd98929c6e420f274c | ok |
| xlibre-x11-common | 1:25.1.9-1+lcos1 | session scripts (see notes) | n/a | 18392 | 2f977684848deb6861d1d85cbbcc53a5d0c993f950f428ac45e6420355c15156 | ok |
| xserver-xlibre-input-libinput | 25.0.1-1+lcos1 | xlibre-xf86-input-libinput-25.0.1 | input-26 | 43408 | 117afb33c0bbc2ac538beed60db7cea17b2b8425a08fcc1d6f41ce45f87ea2ec | ok |
| xserver-xlibre-input-all | 1:25.1.9-1+lcos1 | metapackage (Depends: libinput) | n/a | 1372 | f9f97b32ea398c363f9f844b4a91cca1b958207535b7ecbcf49277c0c709b77d | ok |
| xserver-xlibre-video-amdgpu | 25.1.2-1+lcos1 | xlibre-xf86-video-amdgpu-25.1.2 | video-28 | 71612 | 3e03da6ec399fadcf588f55f61504b3fc13124cbd770b153811b8afbd2bd5c67 | ok |
| xserver-xlibre-video-ati | 1:25.0.1-1+lcos1 | xlibre-xf86-video-ati-25.0.1 | video-28 | 146972 | d6dbccaea6995a0fb88fa3c0f4efd4aef92bbf4d384672ef40195c88831f421a | ok (ati+radeon) |
| xserver-xlibre-video-fbdev | 1:25.0.0-1+lcos1 | xlibre-xf86-video-fbdev-25.0.0 | video-28 | 10400 | d3e287036d8ec34495e8b261e72b7781d99a5331937c73c466d8db66a2b66389 | ok |
| xserver-xlibre-video-nouveau | 1:25.0.1-1+lcos1 | xlibre-xf86-video-nouveau-25.0.1 | video-28 | 85232 | b9d8acb02f60f8c9dcd74d6adcf7d27558b5e29ba53cbbb8e7eab8473ebd027d | ok |
| xserver-xlibre-video-vesa | 1:25.0.0-1+lcos1 | xlibre-xf86-video-vesa-25.0.0 | video-28 | 12708 | 6edf0cfd6c242759592e23b8bd74594fd320e12aa31c2f5f45fb09823a1c08e9 | ok |
| xserver-xlibre-video-vmware | 1:25.0.0-2+lcos1 | xlibre-xf86-video-vmware-25.0.0 | video-28 | 72620 | a5a920a8ad076458e06e5d5b7304e64856477e4a0d2fa75f1830c0d129ca6011 | ok (vmwgfx/KMS) |
| xserver-xlibre-video-all | 1:25.1.9-1+lcos1 | metapackage | n/a | 1416 | ee84ab83cb0a757325f3cbb45d6696b5cfc4e088dedc465eb77305f01e108fbb | ok |

`config/package-lists/xlibre.list.chroot` still lists names only so `lb`
installs these debs. Epochs kept where the old package had them (server
epoch 2; ati/fbdev/nouveau/vesa/vmware epoch 1) so versions sort newer
than 2:25.0.0.12-1 / 1:7.8+4 / 1:22.0.0.3-1.

## What failed / skipped / notes

| Item | Result |
|------|--------|
| Server 25.1.9 | Built (meson). `XLibre X Server 1.25.1.9` reported by `Xorg -version`. |
| libinput 25.0.1 | Built (meson) into `…/xlibre-25/input/libinput_drv.so`. |
| amdgpu 25.1.2 | Built (meson). Default meson `moduledir` is `xorg/modules`; rebuilt with `-Dmoduledir=xorg/modules/xlibre-25` so it matches the 25.1 loader path. |
| ati 25.0.1 | Built (autotools) against 25.1.9 ABI. **No 25.1 tag exists**; 25.0.1 is the latest xlibre tag and it compiled. Ships **both** `ati_drv.so` and `radeon_drv.so` in this one package (no invented xf86-video-radeon repo/package). Old 25.0.0.12 ati wrapper depended on a missing `xserver-xlibre-video-radeon`. |
| fbdev / nouveau / vesa | Built (autotools). |
| vmware 25.0.0 | Rebuilt 2026-08-25 (ET) with vmwgfx/KMS as `1:25.0.0-2+lcos1`. This tag’s `configure.ac` never assigns `BUILD_VMWGFX=yes` (no `--enable-vmwgfx` flag). Forced with `export BUILD_VMWGFX=yes` before `./configure --with-xorg-module-dir=/usr/lib/xorg/modules/xlibre-25 --enable-vmwarectrl-client`. libdrm / xatracker / libudev already present in the Excalibur chroot; configure printed “whether to build Kernel Mode Setting and 3D: yes” and `config.h` has `#define BUILD_VMWGFX 1`. Stripped `vmware_drv.so` is 178584 B (2D-only was 65224 B) and links libxatracker2 / libdrm2 / libeudev1. Also ships `vmwarectrl`. Old 2D deb kept as `packaging/debs/xlibre/xserver-xlibre-video-vmware_25.0.0-1+lcos1.2d-only.deb`. No `xlibre-xf86-video-vmware-25.1.*` tag exists (only 25.0.0). |
| intel, qxl, evdev, wacom, … | Skipped (not needed; video-all Depends only on drivers actually built). |
| Xephyr / Xvfb / Xnest / -dev / -dbgsym / -legacy | Not packaged. |
| xlibre-x11-common files | Session infrastructure (`Xsession`, `Xsession.d`, `rgb.txt`, init script) reused from the old 7.8+4 vendor package (not ABI-dependent; xserver source does not ship them). Repackaged 1:25.1.9-1+lcos1. Dropped the old `changelog.xorg.old.gz`, which is why the deb is ~18 KiB vs ~215 KiB. |
| Trixie host build | Not used. Host `apt` on Trixie had a security/base version skew (`libxfont2` 1:2.0.6-1+deb13u1 vs -dev 1:2.0.6-1+b3); Excalibur chroot avoided that. |
| Install test | In the Excalibur chroot: all 13 runtime/meta debs (everything except the `xlibre` fonts/apps metapackage) `dpkg -i` + configure together. `dpkg -C` clean. `xlibre-x11-common` Conflicts/Provides `x11-common` as designed (had to remove Debian `x11-common` first in the build chroot because `keyboard-configuration` had pulled it). No leftover Depends on 25.0.0.12 packages. |

## video-all Depends (built set)

`xserver-xlibre-video-amdgpu, xserver-xlibre-video-ati, xserver-xlibre-video-fbdev, xserver-xlibre-video-nouveau, xserver-xlibre-video-vesa, xserver-xlibre-video-vmware`

## Module layout on disk

```
/usr/lib/xorg/modules/xlibre-25/
  libglamoregl.so libexa.so libfbdevhw.so libint10.so
  libshadow.so libshadowfb.so libvgahw.so libwfb.so
  extensions/libglx.so
  drivers/modesetting_drv.so          (from core)
  drivers/{amdgpu,ati,radeon,fbdev,nouveau,vesa,vmware}_drv.so
  input/libinput_drv.so
  input/inputtest_drv.so              (from core; unused)
```

xlibre-debian `debian/` from `xlibre-debian/xlibre-server` was inspected as a
**template** only (file split, Provides). Their apt source was not added.

## SHA256SUMS

See `packaging/debs/xlibre/SHA256SUMS` (same bytes as `config/packages.chroot/`).

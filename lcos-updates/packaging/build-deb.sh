#!/bin/sh
# Build lcos-updates_0.1-1_amd64.deb and drop it in the LCOS overlay dirs.
set -eu

ROOT="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
VERSION="0.1-1"
PKGNAME="lcos-updates_${VERSION}_amd64"
OVERLAY="/workspace/lcos-live-03"
BUILD="$ROOT/build"
DEST="$OVERLAY/packaging/src/lcos-updates"
DEB_DIR="$OVERLAY/packaging/debs"
CHROOT_DIR="$OVERLAY/config/packages.chroot"

cd "$ROOT"

rm -rf "$BUILD"
meson setup "$BUILD" --prefix=/usr --buildtype=release -Dstrip=true
meson compile -C "$BUILD"
meson test -C "$BUILD" --print-errorlogs

rm -rf "$DEST"
meson install -C "$BUILD" --destdir "$DEST"

mkdir -p "$DEST/debian"
cp "$ROOT/debian/control" "$DEST/debian/control"

# dpkg-shlibdeps needs debian/control in cwd.
SHLIBS="$(
  cd "$DEST"
  dpkg-shlibdeps --ignore-missing-info -O \
    -e usr/bin/lcos-updates \
    -e usr/libexec/lcos-updates-helper
)"
# shlibs:Depends=foo, bar
SHLIBS_DEPS="${SHLIBS#shlibs:Depends=}"

# Installed-Size is KiB.
SIZE="$(du -sk "$DEST/usr" | awk '{print $1}')"

mkdir -p "$DEST/DEBIAN"
cat > "$DEST/DEBIAN/control" << CTRL
Package: lcos-updates
Version: ${VERSION}
Section: admin
Priority: optional
Architecture: amd64
Installed-Size: ${SIZE}
Maintainer: LCOS <lcos@lunduke.com>
Homepage: https://lunduke.com
Depends: ${SHLIBS_DEPS}, policykit-1, apt, desktop-file-utils
Description: Check for updates from Devuan and LCOS
 A small GTK3 tool that checks for and installs apt upgrades
 (not dist-upgrade) via a polkit helper. No daemon, no tray.
CTRL

cat > "$DEST/DEBIAN/postinst" << 'POST'
#!/bin/sh
set -e
if [ "$1" = configure ]; then
  update-desktop-database /usr/share/applications >/dev/null 2>&1 || true
fi
exit 0
POST
chmod 0755 "$DEST/DEBIAN/postinst"

(
  cd "$DEST"
  find usr -type f -print0 | sort -z | xargs -0 md5sum > DEBIAN/md5sums
)

rm -rf "$DEST/debian"

mkdir -p "$DEB_DIR" "$CHROOT_DIR"
fakeroot dpkg-deb --root-owner-group --build "$DEST" "$DEB_DIR/${PKGNAME}.deb"
cp -f "$DEB_DIR/${PKGNAME}.deb" "$CHROOT_DIR/${PKGNAME}.deb"

echo "built $DEB_DIR/${PKGNAME}.deb"

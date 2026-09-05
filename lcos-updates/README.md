# Check for updates… (`lcos-updates`)

GTK3 / gtkmm-3.0 tool for LCOS. Checks for and installs `apt-get upgrade`
updates from Devuan and LCOS. No daemon, no tray, no PackageKit, no
`dist-upgrade`.

The GUI (`/usr/bin/lcos-updates`) never runs as root. Privileged work is
`pkexec /usr/libexec/lcos-updates-helper` with argv `simulate` or `upgrade`.

License: GPL-3.0-or-later

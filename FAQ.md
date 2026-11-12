# LCOS FAQ

Short answers to common questions about the Lunduke Computer Operating System (LCOS).

## Contents

1. [About LCOS](#about-lcos)
2. [System requirements](#system-requirements)
3. [Download and install](#download-and-install)
4. [Boot problems](#boot-problems)
5. [Everyday use](#everyday-use)
6. [Software and drivers](#software-and-drivers)
7. [Virtual machines](#virtual-machines)
8. [Help and source code](#help-and-source-code)

---

## About LCOS

### What is LCOS built on?

The Lunduke Computer Operating System (LCOS) adheres to a core set of ideas and values:

- No Age or ID Verification.
- No Online Accounts Needed.
- No Built-in AI Features.
- No Weird Politics.
- No Systemd.
- No Forced Rust Clones.
- Modern Tech. 90s UI.

LCOS is built on top of Devuan (a high quality fork of Debian, which removes Systemd), with a hard fork of the Linux Kernel (The Lunduke Linux Kernel) focusing on speed and removing unnecessary portions (such as Rust).  LCOS includes carefully selected set of software (including the stripped-down Brave Origin as the default web browser), along with a collection of LCOS-specific software, which adheres to the LCOS values.

### Is there a dark theme?

No.  You may install any theme you wish, including dark themes, but LCOS does not ship with one.

---

## System requirements

| | |
|---|---|
| **Processor** | 64-bit x86 (amd64). No 32-bit or ARM version. |
| **Firmware** | BIOS or UEFI, including 32-bit UEFI (some older Macs and tablets). |
| **Secure Boot** | Must be off. The LCOS kernel isn't signed for Secure Boot ([how to turn it off](#the-usb-stick-shows-bad-shim-signature-and-wont-boot)). |
| **USB stick** | 2 GB or larger (the ISO is roughly 1.5 GB). |
| **Graphics** | Intel, AMD and NVIDIA work with the built-in open-source drivers. |

| | Minimum | Recommended |
|---|---|---|
| **Memory (RAM)** | 2 GB | 4 GB |
| **Disk** | 10 GB | 20 GB or more |

A fresh install uses about 3.6 GB of disk. 2 GB of RAM runs the desktop, a browser with a few tabs and an editor, leaving roughly 1 GB free (depending on your hardware configuration). There is no swap by default, so running out of memory closes a program rather than slowing the system down.

---

## Download and install

### Where do I download LCOS?

You can grab the latest LCOS ISO at either:

- [Lunduke.com](https://lunduke.com/lcos.html).
- The GitHub [Releases page](https://github.com/BryanLunduke/LCOS/releases).

### How do I check the download?

Compare the file's SHA256 checksum with the one listed next to the ISO on the Releases page. In the commands below, replace `lcos-NNN.iso` with the name of the file you downloaded.

| System | Command |
|---|---|
| Linux | `sha256sum lcos-NNN.iso` |
| macOS | `shasum -a 256 lcos-NNN.iso` |
| Windows | `certutil -hashfile lcos-NNN.iso SHA256` |

### How do I make a bootable USB stick?

Write the ISO to the stick as-is with any "raw image" or "DD mode" writer. This erases the stick.

- **Windows and macOS:** [balenaEtcher](https://etcher.balena.io/) writes the ISO directly. On Windows, [Rufus](https://rufus.ie/) also works, but choose **DD Image mode** when it asks.
- **Ventoy:** also works. LCOS boots from a Ventoy stick.
- **Linux:** replace `sdX` with your USB stick:
  ```sh
  sudo dd if=lcos-NNN.iso of=/dev/sdX bs=4M status=progress conv=fsync
  ```

### How do I install LCOS?

Boot from the USB stick to get the live desktop, then run the installer (on the desktop) from there. It doesn't need an internet connection.

### What is the live session password?

User `lcos`, password `live`.

The live session logs in automatically, so you'll only need it if the screen locks, something asks for a password, or you land at a text login.

An installed system doesn't use this password; it uses the username and password you choose in the installer.

### Does LCOS support disk encryption?

Yes. Choose **Encrypt system** in the installer (LUKS). You enter the passphrase once at boot.

### Which file systems are supported?

- **Installer default:** ext4.
- **Also supported by the kernel:** Btrfs, XFS, FAT, exFAT, NTFS and UDF, so drives in those formats work.

---

## Boot problems

### The USB stick shows "bad shim signature" and won't boot

That error (often followed by "you need to load the kernel first") means Secure Boot is on, and LCOS doesn't support it.

1. Open the bios/firmware setup at power-on (usually **F2**, **F12**, **Del** or **Esc**).
2. Turn off **Secure Boot** and save.
3. Boot the stick again.

You can ignore any "null src bitmap" line.

### The screen is black after booting

Pick a safe graphics entry from the boot menu:

- **UEFI:** "LCOS (safe graphics)" or "fail-safe mode"
- **BIOS:** "LCOS live (amd64 fail-safe mode)"

If that works, please [report](#where-do-i-report-a-bug-or-ask-a-question) your graphics card.

---

## Everyday use

### How do I update LCOS?

Open **System > Check for updates…** and click **Check**, or run:

```sh
sudo apt update && sudo apt full-upgrade
```

### How do I turn on the screensaver and screen lock?

Both are off by default. Turn them on in **Settings > Xfce Screensaver**. You can always lock the screen by hand from the user menu at the top right.

### How do I turn automatic login on or off?

Installed systems log in automatically by default. To turn this off during installation, untick the automatic login box on the installer's **Users** page.

After installation, edit both LightDM files below (`lightdm.conf` is read last and wins).

**To turn it off:**

1. Open a terminal and run:
   ```sh
   sudo sed -i 's/^autologin-user=/#autologin-user=/' /etc/lightdm/lightdm.conf /etc/lightdm/lightdm.conf.d/50-lcos.conf
   ```
2. Reboot. At the login screen, type your username (the user list is hidden), then your password.

To check, run `grep -n 'autologin-user=' /etc/lightdm/lightdm.conf /etc/lightdm/lightdm.conf.d/50-lcos.conf`. Every line should start with `#`.

**To turn it back on:**

1. Run:
   ```sh
   sudo sed -i 's/^#autologin-user=\(.\+\)/autologin-user=\1/' /etc/lightdm/lightdm.conf /etc/lightdm/lightdm.conf.d/50-lcos.conf
   ```
2. Reboot.

### How do I change the time zone or time?

You choose the time zone in the installer. After that, LCOS keeps the clock correct automatically over the internet (chrony), so you normally never need to set it.

- **Change the time zone:** run `sudo dpkg-reconfigure tzdata` and pick your region and city. It takes effect right away; log out and back in if the panel clock still shows the old time.
- **Set the time by hand** (rarely needed): for example, `sudo date -s "2026-10-23 11:15"`. While you're online, LCOS corrects the clock again automatically.

The panel clock's **Properties** (right-click the clock) can show a different time zone, but that only changes the display, not the system time zone.

---

## Software and drivers

### How do I install Steam?

The easiest way is Valve's own package:

1. Open **Software > System > Check for updates...** and enter your password, or run `sudo apt update` once. A fresh install has no package lists yet, so until you do, GDebi shows "Dependency is not satisfiable: xterm|gnome-terminal|konsole" and the Install button stays greyed out.
2. Download the `.deb` from [store.steampowered.com/about](https://store.steampowered.com/about/), then double-click it to install it with GDebi. You can also run `sudo apt install ./steam_latest.deb` in the folder you saved it to.
3. Open Steam. A window titled "Evaluating dependencies" asks you to press Return, then asks for your password. Steam installs the 32-bit libraries it needs (press Return or Y when asked), then shows the Steam sign-in window.

You can also install Debian's `steam-installer` package. It needs 32-bit (i386) packages turned on first:

```sh
sudo dpkg --add-architecture i386
sudo apt update
sudo apt install steam-installer
```

### How do I install NVIDIA's proprietary driver?

For most users, with most NVIDIA GPUss, you probably don't need to install the proprietary driver.

LCOS uses the built-in open-source **nouveau** driver (with Vulkan through NVK) by default, with no necessary setup. NVIDIA's driver is only needed for things like CUDA or the best gaming performance (in some configs). It is proprietary: LCOS doesn't ship it, and you download it from NVIDIA under their license.

**Supported cards:** Turing or newer — GTX 16xx, RTX 20/30/40/50 and Turing-or-newer workstation cards. Pascal or older (GTX 10xx and earlier) should stay on nouveau: the open kernel modules don't support them, and the older proprietary drivers don't build on kernel 7.2. To check your GPU, run `lspci -nn | grep -iE 'vga|3d|display'`.

**Driver version:** use NVIDIA's own **580-series or newer** driver. Don't install Devuan's `nvidia-driver` package (version 550); it doesn't build on the custom Lunduke Kernel.

**Before you start:** turn off Secure Boot (if it's on), install all updates and back up your files. Keep these steps on another device, because you'll leave the desktop. If the screen goes black at any point, press **Ctrl+Alt+F2** for a text login.

1. Remove any Devuan NVIDIA packages:
   ```sh
   sudo apt purge 'nvidia-*' 'libnvidia-*' 'xserver-xorg-video-nvidia*'
   sudo apt -f install && sudo apt autoremove --purge
   ```
2. Install the build tools. The kernel headers are usually pre-installed, and their version must match the kernel:
   ```sh
   sudo apt update
   sudo apt install build-essential dkms linux-headers-7.2.6-lunduke pkg-config libglvnd-dev
   ```
3. Turn off nouveau:
   ```sh
   printf 'blacklist nouveau\noptions nouveau modeset=0\n' | sudo tee /etc/modprobe.d/blacklist-nouveau.conf
   sudo update-initramfs -u
   ```
4. Download the latest 580-or-newer Linux 64-bit driver (`NVIDIA-Linux-x86_64-<version>.run`) from [nvidia.com/drivers](https://www.nvidia.com/en-us/drivers/).
5. Press **Ctrl+Alt+F2**, log in, stop the desktop and run the installer:
   ```sh
   sudo service lightdm stop
   sudo sh ./NVIDIA-Linux-x86_64-*.run --dkms --kernel-module-type=open
   ```
   Answer the installer's questions:
   - Kernel modules: **MIT/GPL** (open)
   - DKMS: **Yes**
   - 32-bit libraries: **Yes** if you use Steam or Wine
   - `nvidia-xconfig`: **No** (LCOS already loads the NVIDIA X driver automatically)
6. Rebuild the initramfs and reboot, then run `nvidia-smi` to check the driver works:
   ```sh
   sudo update-initramfs -u && sudo reboot
   ```

**After a kernel update:** LCOS kernel updates keep the same kernel name (`7.2.6-lunduke`), so DKMS may skip the rebuild. After any update to `linux-image-7.2.6-lunduke`, run this before you reboot:

```sh
sudo dkms install nvidia/$(dkms status | grep -o 'nvidia/[0-9.]*' | head -1 | cut -d/ -f2) -k 7.2.6-lunduke --force
sudo update-initramfs -u
```

**To undo it or recover from a black screen:** at a text login (**Ctrl+Alt+F2**), run:

```sh
sudo service lightdm stop
sudo nvidia-installer --uninstall
sudo rm -f /etc/modprobe.d/blacklist-nouveau.conf /etc/modprobe.d/nvidia-installer-disable-nouveau.conf
sudo update-initramfs -u && sudo reboot
```

If Ctrl+Alt+F2 doesn't work, press **e** at the GRUB menu, add `3 nomodeset` to the end of the `linux` line, and press **F10**.

---

## Virtual machines

### Does LCOS work in a virtual machine?

Yes. LCOS includes `open-vm-tools` and `spice-vdagent`.

- **QEMU/KVM:** use **virtio** or **std** graphics. VMware SVGA graphics doesn't start the desktop, and QXL is unreliable.
- **VirtualBox:** the mouse cursor may be invisible using some graphics options.

### Can I run virtual machines on LCOS?

Yes. The kernel supports KVM.

To use **VirtualBox**, install it from Oracle's website or repository as usual (the Debian "trixie" package works). Before starting your first VM, build its kernel driver (the matching LCOS kernel headers are already installed):

```sh
sudo apt install gcc make
sudo /sbin/vboxconfig
```

If you skip this, VirtualBox shows "Kernel driver not installed (rc=-1908)" when you start a VM. If that error appears after a kernel update, run `sudo /sbin/vboxconfig` again.

---

## Help and source code

### Where do I report a bug or ask a question?

- **Bugs:** [GitHub Issues](https://github.com/BryanLunduke/LCOS/issues). Include your computer model, graphics card and LCOS version.
- **Questions and suggestions:** [The Lunduke Journal Forum](https://forum.lunduke.com/), open to [Lunduke Journal subscribers](https://lunduke.com/).

### Where is the source code?

- **Packages:** each release has a package source list (for example [LCOS-09-PackageSourceList.md](https://github.com/BryanLunduke/LCOS/blob/master/LCOS-09-PackageSourceList.md)) linking to the source of every package.
- **ISO build recipe:** [lcos-live](https://github.com/BryanLunduke/lcos-live)
- **Kernel:** [lunduke-linux-kernel](https://github.com/BryanLunduke/lunduke-linux-kernel)

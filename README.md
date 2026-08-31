# Selenite-OS
A Homemade OS! Made by Dani! (note we are still in beta 0.8 so please expect us to be done from 18 october to like idk 4 years after. Oh yeah everything is open source! 


The minimal expected specs and recommended specs. (not sure)

CPU AMD X86_64 CPU (we will do intel later)
GPU idk just intergraded i guess. AMD ONLY! intel arc series or nvidia will do later (nvidia prolly not but if they are open source then yes)
RAM 1gb DDR3 minimum recommended is 4GB DDR4 2133

Any question? Ask me! The ISO is for QEMU purposes only! Please dont install on your pc nor doing it on your pc for fun. Its only for qemu. also serial is needed or else you will only see the uefi screen (tiano core) thats why you cannot use it if you wanna ask.

qemu code! copy and paste it.

qemu-system-x86_64 \
  -machine q35 \
  -m 512M \
  -display gtk \
  -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE_4M.fd \
  -drive if=pflash,format=raw,file=build/OVMF_VARS.fd \
  -drive format=raw,file=build/subs.img \
  -serial stdio \
  -net none

  if you didnt install qemu? install using this command

  debian ubuntu mint pop os or any other os that uses the same as debian
  
  sudo apt update && sudo apt install qemu-system qemu-kvm virt-manager libvirt-daemon-system

  fedora and other based on that

  sudo dnf install @virtualization

rocky linux (for the 3 people that uses this. NO INSULT INTENDED)

  sudo dnf groupinstall "Virtualization Host"

  arch linux or artix and others based on that. (i use arch btw)

  sudo pacman -Syu qemu-full virt-manager libvirt dnsmasq

  opensuse leap and thunderweed 

  sudo zypper in -t pattern kvm_server kvm_tools

  for all the other os that uses there own package

  Alpine Linux: apk add qemu-system-x86_64 qemu-kvm
  Void Linux: sudo xbps-install -S qemu libvirt virt-manager
  Solus: sudo eopkg it qemu
  gentoo: emerge --ask app-emulation/qemu
  Nix os: installation: Add virtualisation.libvirtd.enable = true; and programs.virt-manager.enable = true; to your configuration.nix file.
  Steam os: (For all the steam machine users) Installation: Because its file system is locked by default, you have to disable read-only mode (sudo steamos-readonly disable) before using Arch's pacman command to install QEMU.
  Slackware: command: QEMU is usually available on SlackBuilds. You build it using script files: sbopkg -i qemu

  # for the lfs nerds 

Linux From Scratch (LFS) does not have a package manager, so you must compile QEMU entirely from source code. In the LFS ecosystem, QEMU is documented under the BLFS (Beyond Linux From Scratch) book.1. Prerequisite DependenciesBefore building QEMU, your LFS system must already have GLib, Pixman, and Ninja installed.If you want a graphical interface or audio inside your virtual machines, you should also install SDL2 or GTK+3, and alsa-lib before configuring QEMU.2. Compilation and Installation StepsRun the following commands as a standard user to download, extract, configure, and compile QEMU:bash# Download the official QEMU source archive
wget https://qemu.org
tar -xf qemu-10.0.3.tar.xz
cd qemu-10.0.3

 Create an isolated build directory
mkdir build
cd build

Configure the build system (Enables KVM acceleration)
../configure --prefix=/usr \
             --sysconfdir=/etc \
             --localstatedir=/var \
             --docdir=/usr/share/doc/qemu-10.0.3 \
             --enable-kvm

 Compile using all available CPU cores
make
Once the compilation completes successfully, switch to the root user to install it:bashsudo make install
3. Activating Permissions (KVM Group)LFS relies on standard kernel structures. To make sure your user profile can utilize hardware virtualization without typing sudo every time, configure permissions for the KVM device node:bash# Ensure the kvm group exists and append your username
sudo groupadd -f kvm
sudo usermod -a -G kvm <your_lfs_username>

okay thats it i dont wanna do more haha

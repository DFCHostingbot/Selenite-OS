<div align="center">

# 📱 SelenyxOS

**A custom mobile operating system for Selenyx 1**

[![Build Status](https://img.shields.io/badge/build-scaffold-blue)]()
[![License](https://img.shields.io/badge/license-Proprietary%20%2B%20GPL--2.0-orange)]()
[![Platform](https://img.shields.io/badge/platform-ARM64-green)]()
[![QEMU](https://img.shields.io/badge/emulation-QEMU%20virt-yellow)]()

</div>

> ⚠️ **SelenyxOS does not use the Linux kernel.**
> The kernel is a fully custom kernel named **selenyx-kernel**. Linux-derived code is allowed *only* for selected drivers under `drivers/linux/`, executed through a Selenyx-native compatibility layer called **lxcompat**.

---

## 🎯 First Milestone

Minimal QEMU-bootable ARM64 system with:
- Simple phone shell
- USB 4G dongle detection and usage
- Linux-derived drivers via lxcompat

---

## 🖥️ Target Hardware

| Component | Specification |
|-----------|---------------|
| Device | Selenyx 1 |
| Kernel | selenyx-kernel (custom, non-Linux) |
| SoC | Allwinner A733 |
| CPU | ARM64 (QEMU `virt` first) |
| RAM | 6 GB LPDDR5 4800 MHz |
| Display | 5″ 960×600 @ 60 Hz |
| Camera | 12 MP single rear |
| Connectivity | External USB 4G dongle |
| GPU | Selenyx 8 3700x1d3 (custom) |
| Input | Touchscreen, power, volume |
| Storage | eMMC / SD |

> 💡 Custom GPU is not emulated in QEMU. Virtual framebuffer is used initially.

---

## 🏗️ Architecture Policy

### Custom Kernel (`kernel/`)
ARM64 boot, scheduler, MM, IPC, VFS+devfs, framebuffer console, input, USB host stack, network stack, driver manager, native drivers. **No Linux kernel core.**

### Linux Driver Compatibility Layer (`lxcompat/`)
Selenyx-native reimplementation of Linux-style APIs. NOT Linux code. Enables minimal adaptation of real Linux driver sources.

**APIs provided:**
- Memory: `kmalloc`/`kfree`/`kzalloc`
- Sync: spinlocks, mutexes, wait queues
- Async: workqueues, timers, kthreads
- Core: `printk`, module init/exit, device model
- Subsystems: USB core, `net_device`, TTY/serial, firmware stubs

### Driver Policy (`drivers/`)
| Directory | Contents |
|-----------|----------|
| `drivers/linux/` | Adapted Linux drivers only |
| `drivers/selenyx/` | Native Selenyx drivers |

### Userspace (`userspace/`)
| Component | Purpose |
|-----------|---------|
| `init/` | System initialization |
| `shell/` | Phone shell |
| `settings/` | Configuration UI/logic |
| `netd/` | DHCP, DNS, interface management |
| `4gd/` | 4G modem daemon |

---

## 📁 Source Layout

SelenyxOS/
├── kernel/{arch/arm64,mm,sched,fs,drivers,net,ipc,init}/
├── lxcompat/{include/linux,core,usb,net}/
├── drivers/{linux/{usb,net},selenyx}/
├── userspace/{init,shell,settings,netd,4gd}/
├── tools/
├── qemu/
└── docs/


---

## 🔨 Build (Planned)

```sh
export CROSS_COMPILE=aarch64-none-elf-
make defconfig && make -j$(nproc)
# Output: build/selenyxos.img

prerequisites: AArch64 cross-toolchain, GNU Make, QEMU ≥ 8.0, host USB permissions for passthrough.

🚀 Boot in QEMU
Serial-only bring-up
qemu-system-aarch64 -machine virt,highmem=on -cpu cortex-a72 \
  -smp 4 -m 2048 -serial mon:stdio -kernel build/selenyxos.img

Display + USB Host + Virtual USB NIC

qemu-system-aarch64 -machine virt,highmem=on -cpu cortex-a72 \
  -smp 4 -m 2048 -serial mon:stdio -display gtk \
  -device virtio-gpu-pci \
  -device qemu-xhci,id=xhci \
  -device usb-net,bus=xhci.0,netdev=net0 \
  -netdev user,id=net0 \
  -kernel build/selenyxos.img


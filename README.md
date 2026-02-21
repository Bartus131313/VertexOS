# VertexOS
*A high-precision, TUI-centric 32-bit x86 Operating System.*

VertexOS is a modular, monolithic kernel designed for efficiency and a streamlined user experience. Inspired by modern terminal editors like Neovim, it features a fixed-layout terminal interface, a custom command shell, and direct hardware communication via the Multiboot standard.

## 🛠 Features
- UI: A Neovim-inspired Terminal User Interface with a protected status bar and scrolling "View" region.
- Modular Architecture: Organized into logical domains (drivers/, cpu/, ui/, kernel/).
- Command Shell: Robust input buffering with support for real-time backspacing and line tracking.
- Hardware Intelligence: Detects CPU vendor and calculates total RAM via Multiboot structures.
- Professional Formatting: 24-hour RTC clock with leading-zero padding.

## 🚀 Getting Started
At this moment the only supported system for building is **Linux**. If you are using **Windows** i recommend downloading **Windows Subsystem for Linux (WSL)**
1. Install all requirements:
```Bash
sudo apt update && sudo apt install -y nasm make gcc-multilib binutils qemu-system-x86 grub-common grub-pc-bin xorriso mtools
```

2. Clone the project:
```Bash
git clone https://github.com/Bartus131313/VertexOS.git
cd VertexOS
```
3. Compile and Run:
```Bash
make run
```

## 📜 Commands
- `help` - List available system functions.
- `datetime` - Displays the current date and system time (UTC+1).
- `meminfo` - Queries the Multiboot header for hardware RAM specs.
- `cpu` - Prints the processor's vendor identity.
- `clear` - Resets the terminal buffer.

## ⚖️ License
This project is licensed under the MIT License. You are free to use, modify, and distribute this software, provided that the original copyright notice and permission notice are included.
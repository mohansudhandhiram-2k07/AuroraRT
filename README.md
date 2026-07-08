# AuroraRT

**Status: ACTIVE — Month 02 of 12**  
**Target:** STM32F1 (Cortex-M3) / Porting to Cortex-M4 later  
**Architecture:** Bare-metal, pure C, custom build system  

AuroraRT is a personal engineering project to build a Real-Time Operating System entirely from scratch. 

Most embedded portfolio projects glue together vendor HALs (Hardware Abstraction Layers), Arduino libraries, and FreeRTOS. This project takes the opposite approach: **Zero third-party code.** Every line of code, from the reset vector to the context-switching scheduler, is written by reading the raw silicon reference manuals. 

## 🛑 The Hard Rules
1. **No Vendor HAL:** All hardware manipulation is done via memory-mapped pointers and bitwise operations.
2. **No Standard C Library:** No `<stdio.h>`, no `<string.h>`. Functions like `printf`, `memcpy`, and `malloc` are built from scratch.
3. **No IDE Magic:** Built using raw `arm-none-eabi-gcc` and a custom `Makefile`. 
4. **The Notebook Rule:** Every design decision, hardware bug, and memory architecture choice is documented in the `/docs/journal` directory.

## 🧱 The Stack (Top to Bottom)
- [ ] **Layer 7: Applications** (Sensor fusion, Real-time dashboards)
- [ ] **Layer 6: Math** (Custom vector, matrix, and quaternion libraries)
- [ ] **Layer 5: Middleware** (CLI, Ring Buffers, Logging)
- [ ] **Layer 4: Kernel** (Scheduler, Mutex, Semaphores, Context Switching)
- [ ] **Layer 3: Drivers** (SPI, I2C, Timers, DMA)
- [x] **Layer 2: Custom Libc** (Variadic `printf`, string manipulation) *-- In Progress*
- [x] **Layer 1: Boot & Hardware Setup** (Linker script, Startup `.s`/`.c`, basic GPIO)

## 📖 Development Journal
This repository serves as an engineering notebook. You can track the architectural decisions and debugging process layer by layer:
* [Month 01: Boot Process, Linker, and Bare-Metal GPIO](docs/journal/month_01_boot.md)
* [Month 02: UART & Custom Libc](docs/journal/month_02_libc_uart.md)

## 🛠️ Build Instructions
```bash
# Clone the repository
git clone https://github.com/yourusername/AuroraRT.git
cd AuroraRT

# Build the firmware (Requires arm-none-eabi-gcc)
make

# Output will be generated in the build/ directory
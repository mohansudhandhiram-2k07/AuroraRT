# Month 01: Boot Sequence & Memory Map

**Dates:** July 5 - July 6, 2026  
**Goal:** Write a custom linker script and startup file from scratch to boot an STM32F401RE and reach `main()` without using any ST HAL or standard libraries.

## 1. Hardware & Memory Mapping
To write the linker script, I first had to dig into the STM32F4 Reference Manual and Datasheet to find the physical memory boundaries. 

* **FLASH:** Starts at `0x0800 0000` (1 MB)
* **SRAM:** Starts at `0x2000 0000` (128 KB total)

*Note on SRAM:* The datasheet lists SRAM1 (112 KB) and SRAM2 (16 KB) separately. However, looking at the addresses, SRAM1 ends at `0x2001 BFFF` and SRAM2 starts at `0x2001 C000`. Because they are physically contiguous, I combined them into a single 128 KB `SRAM` block in my linker script (`linker.ld`) to prevent the linker from artificially restricting my `.bss` or `.data` sizes. 
* **Stack Pointer:** Calculated the top of RAM (`_estack`) by adding 128KB (`0x20000`) to the base address: `0x2002 0000`.

## 2. Linker Script Challenges (GNU LD)
I learned quickly that the GNU Linker is incredibly pedantic about whitespace. 
* **Bug:** `FLASH(rx):` threw a syntax error.
* **Fix:** It must be formatted as `FLASH (rx) :` with spaces between the tokens. 

* **The LMA vs VMA problem:** I mapped `.data` to run in SRAM (VMA) but load in FLASH (LMA). However, my C code needed to know exactly where in Flash that data lived so it could copy it during boot. 
* **The Fix:** I used the linker command `_sidata = LOADADDR(.data);` to expose the Flash address to my `startup.c` file.

## 3. Writing the Reset Handler
I created the absolute minimum Vector Table required by the Cortex-M4 to boot: just the initial Stack Pointer and the `reset_handler` address.

Inside the reset handler, I wrote the loops to initialize memory. I ran into a massive pointer trap here:
* **The Pointer Bug:** I wrote the `.bss` zeroing loop condition as `while(src < _ebss)`. 
* **Why it failed:** `_ebss` is a linker symbol, not a C variable. Calling `_ebss` reads the *value* at that RAM address (which is currently random garbage electrical noise). I was comparing my pointer address to garbage data, which would cause the loop to overwrite random memory and trigger a HardFault.
* **The Fix:** Changed to `while(src < &_ebss)` to compare the memory *addresses*.

## 4. Final Compilation
I compiled the OS using `arm-none-eabi-gcc`. 
* Ran into a `multiple definition of 'main'` error because I accidentally had conflicting definitions of `main` between my files. Cleaned up the declarations.
* **Success:** Used `arm-none-eabi-objdump -h` to verify the `.elf` binary. `.text` was perfectly mapped to `0x08000000`, and `.data` was mapped to run in RAM but load from Flash. 

Layer 1 complete. Next up: The Build System (Makefile).

## 5. Automating the Build (Makefile)
Typing the raw GCC command was getting tedious, so I wrote a professional Makefile to automate the toolchain.
* **Architecture:** Structured the Makefile into Toolchain, Files, Flags, and Rules. Configured it so that all intermediate object files (`.o`) and final binaries (`.elf`, `.bin`, `.map`) output into a clean `build/` directory.
* **Version Control:** Updated `.gitignore` to completely ignore the `build/` folder and dependency tracking files. 

## 6. First Bare-Metal Driver (GPIO Blink)
The final step for Layer 1 was proving the CPU could actually manipulate hardware. No HAL, just raw pointers to memory-mapped registers. 

### The Datasheet Catch (F1 vs F4)
While looking up the memory map in the ST Reference Manual to find the RCC (Reset and Clock Control) and GPIO base addresses, I noticed a discrepancy. The addresses I was expecting for the Cortex-M4 (STM32F4) didn't match the datasheet I was reading. 
* I noticed `0x4002 0000` mapped to DMA1 in my PDF, not GPIOA! 
* **Realization:** I was looking at the STM32F1 (Cortex-M3) Reference Manual. Rather than panicking, I used this as an exercise in hardware abstraction. I decided to pivot and write the code for the F1 silicon using its specific offsets (RCC at `0x40021000`, GPIOA at `0x40010800`). Since I'm using `#define` macros, adapting this to an F4 board later will only require changing a few hex numbers at the top of the file.

### Bitwise Operations
Hardware registers are 32 bits wide, so I couldn't just assign values directly without overwriting other hardware configurations. I used strict read-modify-write bitwise logic:
* **Enable Clock:** Used `|= (1 << 2)` to set Bit 2 in `RCC_APB2ENR`.
* **Set Pin Mode:** Needed to write the binary pattern `0010` to bits 20-23 in `GPIOA_CRL`. First, I cleared the bits using a mask: `&= ~(0xF << 20)`. Then, instead of shifting `2 << 20`, I realized I could elegantly shift `1 << 21` to place the bit in the exact correct slot.
* **Toggle LED:** Used `^= (1 << 5)` on `GPIOA_ODR` in the infinite loop.

### Bugs
* **The Delay Loop Trap:** I wrote a dumb delay loop to slow down the 16MHz processor: `for(volatile int i = 1000000; i < 0; i--);`. The LED wouldn't blink—it just stayed on. I realized my condition `i < 0` evaluated to false immediately since `i` starts at 1,000,000. Changed it to `i > 0`.

**Month 1 is officially complete.** The foundation is rock solid.
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
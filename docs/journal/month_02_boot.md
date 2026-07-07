# Month 02: UART & Custom Libc

**Dates:** July 7, 2026  
**Goal:** Establish a unidirectional communication channel (Tx) to the host PC by writing a bare-metal UART driver, enabling actual text-based debugging before starting the RTOS kernel.

## 1. Hardware Navigation & The Bus Trap
To make the STM32 talk, I needed to configure USART1. 
* **The APB1 vs APB2 Trap:** While searching the Reference Manual memory map, I found USART2 through UART5 starting at `0x4000 4400`. However, these are on the slower APB1 bus. I realized USART1 is the "high-speed" peripheral and actually lives on the APB2 bus at base address `0x4001 3800`.
* **Clocks:** Enabled the clock for both `GPIOA` (Bit 2) and `USART1` (Bit 14) in the `RCC_APB2ENR` register. 

## 2. GPIO Alternate Function (CRH)
UART Tx requires handing control of a pin (PA9) over to the hardware peripheral. This meant configuring the `GPIOA_CRH` register.
* **Decoding the manual:** Each pin in CRH gets 4 bits. I needed `MODE9` to be Output 50MHz (Binary `11`) and `CNF9` to be Alternate Function Push-Pull (Binary `10`).
* **The Math:** Combining them gave `1011`, which is `0xB` in Hex. Because Pin 9's configuration slot starts at bit 4 of the CRH register, I cleared the mask `&= ~(0xF << 4)` and set it using `|= (0xB << 4)`.

## 3. Baud Rate Calculation
The USART hardware needs a clock divider to hit a target Baud Rate of 115,200.
* The system internal clock (HSI) runs at 8 MHz. 
* Formula: `8,000,000 / 115,200 = 69.444`.
* Taking the whole integer `69`, converting it to Hex gave `0x45`, which I wrote directly into the `USART1_BRR` register.

## 4. C Language & Pointer Bugs
Writing the driver exposed a few blind spots in my C mechanics:
* **The Pointer Trap:** I initially defined my registers as `#define USART1_BRR (0x40013800 + 0x08)`. The compiler treated this as a literal number, not an address. **Fix:** Wrapped them in the standard volatile pointer cast: `(*((volatile uint32_t *) (ADDRESS)))`.
* **Bitwise AND vs. OR:** I accidentally used `&` to combine bit shifts `((1 << 2) & (1 << 14))`, which evaluates to `0`. **Fix:** Changed to Bitwise OR `|`.
* **Operator Precedence in Polling:** To wait for the TXE (Transmit Empty) flag, I wrote `while(!(USART_SR >> 7) & 1);`. C evaluated the `!` before the `&`, causing an infinite loop risk. **Fix:** Switched to the standard embedded masking idiom: `while( !(USART_SR & (1 << 7)) );`.

## 5. Architectural Refactoring & The Makefile Boss
* **The Header Trap:** I originally tried to `#include "uart.c"` directly into `main.c`. This is bad practice and leads to multiple definition errors from the Linker. 
* **The Fix:** I split the driver into `uart.h` (the function prototypes/menu) and `uart.c` (the implementation/kitchen), placed in `drivers/inc/` and `drivers/src/` respectively.
* **Makefile VPATH:** After adding `uart.c` to the source list, `make` couldn't find it. I learned about `VPATH`, the search path for the Make tool. Appending `:drivers/src` to the `VPATH` variable allowed the build system to automatically locate and compile the new driver.

The driver compiles cleanly. Next step: Writing a custom `printf` function to actually use this UART link.
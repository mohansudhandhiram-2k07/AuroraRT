#include <stdint.h>

// Base Addresses (STM32F1)
#define RCC_BASE      0x40021000   
#define GPIOA_BASE    0x40010800   

#define RCC_APB2ENR   (*((volatile uint32_t *)(RCC_BASE + 0x18)))
#define GPIOA_CRL     (*((volatile uint32_t *)(GPIOA_BASE + 0x00)))
#define GPIOA_ODR     (*((volatile uint32_t *)(GPIOA_BASE + 0x0C)))

int main(void) {

    RCC_APB2ENR |= (1 << 2);
    GPIOA_CRL &= ~(0xF << 20); 
    GPIOA_CRL |= (1 << 21);
    
    while(1) { 
        GPIOA_ODR ^= (1 << 5);
        for(volatile int i = 1000000; i > 0;i--);
    }

    return 0;
}
#include <stdint.h>
#include "uart.h"

#define RCC_APB2ENR (*((volatile uint32_t *) (0x40021000 + 0x18)))
#define GPIOA_CRH   (*((volatile uint32_t *) (0x40010800 + 0x04)))
#define USART1_CR1  (*((volatile uint32_t *) (0x40013800 + 0x0C)))
#define USART1_BRR  (*((volatile uint32_t *) (0x40013800 + 0x08)))
#define USART_DR    (*((volatile uint32_t *) (0x40013800 + 0x04)))
#define USART_SR    (*((volatile uint32_t *) (0x40013800)))

void uart_init(void)
{
    RCC_APB2ENR  |= ((1 << 2) | (1 << 14));
    GPIOA_CRH &= ~(0xf << 4);
    GPIOA_CRH |= (0xb << 4);
    USART1_BRR = 0x45;
    USART1_CR1 |= ((1 << 3) | (1 << 13));
}

void uart_write_char(char c)
{
    while(!(USART_SR & (1 << 7)));
    USART_DR = c;    
}
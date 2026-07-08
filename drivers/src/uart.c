#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
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

int uart_puts(const char *str)
{
    if(str == NULL) return -1;
    int count = 0;
    while(*str != '\0')
    {
        uart_write_char(*str++);
        count++;
    }
    return count;
}

void uart_print_int(int num)
{
	if(num == 0)
    {
        uart_write_char('0');
        return;
    }
    if(num < 0)
    {
        uart_write_char('-');
        num = -num;                 // -45 % 10 = 5 --> -4, but we already handles '-'.
    }
	char buffer[10];
	int i = 0;
	for(i = 0;num > 0;i++)
    {
        buffer[i] = (num % 10) + '0';
        num /= 10;
    }
    while(i)
    {
        uart_write_char(buffer[--i]);
    }
}

void uart_printf(const char *format,...)
{
    va_list args;
    va_start(args,format);
    while(*format != '\0')
    {
        if(*format == '%')
        {
            format++;
            switch(*format)
            {
                case 'd':
                    uart_print_int(va_arg(args, int));
                    break;
                case 's':
                    uart_puts(va_arg(args, char *));
                    break;
                case 'c':
                    uart_write_char(va_arg(args, int));
                    break;
                case '%':
                    uart_write_char('%');
                    break;
                default:
                    uart_write_char(va_arg(args, int));
                    break;
            }
        }
        else
        {
            uart_write_char(*format);
        }
        format++;
    }
    va_end(args);
}
#ifndef UART_H
#define UART_H

void uart_init(void);
void uart_write_char(char c);
int uart_puts(const char *str);
void uart_print_int(int num);
void uart_printf(const char *format,...);

#endif

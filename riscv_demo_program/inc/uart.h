#ifndef __UART_H_
#define __UART_H_

#include "peripheral.h"

#define UART_BASE 0x10000000
#define UART_TXFIFO ((uint32_t *)(UART_BASE + 0))
#define UART_RXFIFO ((uint32_t *)(UART_BASE + 4))
#define UART_TXCTRL ((uint32_t *)(UART_BASE + 8))
#define UART_RXCTRL ((uint32_t *)(UART_BASE + 12))
#define UART_IE ((uint32_t *)(UART_BASE + 16))
#define UART_IP ((uint32_t *)(UART_BASE + 20))
#define UART_DIV ((uint32_t *)(UART_BASE + 24))

#define UART_TX_EN 1
#define UART_RX_EN 1

#define UART_IE_MASK_TXWM 1
#define UART_IE_MASK_RXWM 2

#define UART_IP_MASK_TXWM 1
#define UART_IP_MASK_RXWM 2 

#define UART_RXFIFO_EMPTY 0x80000000

#define UART0_IRQ 3

void uart_putc(char ch);
unsigned char uart_getc(void);
void uart_init(void);
void uart_puts(const char *s);
void uart_isr(void);

#endif

#include "uart.h"
#include "platform.h"

void uart_putc(char ch) 
{
  write_reg(UART_TXFIFO, ch);
}

unsigned char uart_getc()
{
  uint32_t ret = read_reg(UART_RXFIFO);
  //uint32_t ret = *(uint32_t *)(UART_RXFIFO);
  if(!(ret & UART_RXFIFO_EMPTY))
    return ret;

  return -1;
}

void uart_puts(const char *s)
{
  while (*s) {
    uart_putc(*s++); 
  }
}

void uart_init()
{
  //enable UART
  write_reg(UART_TXCTRL, UART_TX_EN); 
  write_reg(UART_RXCTRL, UART_RX_EN); 

  asm volatile ("csrw mip, 0");

  //enable UART interrupt
  write_reg(UART_IE, UART_IE_MASK_TXWM|UART_IE_MASK_RXWM);
  //write_reg(UART_IE, 1);
}

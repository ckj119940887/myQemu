#include "uart.h"
#include "platform.h"
#include "plic.h"

int start_kernel()
{
  uint32_t ch;

  trap_init();

  plic_init();

  uart_init();

  riscv_print("hello ckj\n");

  while (!((ch = uart_getc()) & UART_RXFIFO_EMPTY)) {
    uart_putc((char)ch); 
  }

  while (1) {
  }

  return 0;
}


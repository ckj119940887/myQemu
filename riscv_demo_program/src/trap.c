#include "platform.h"
#include "uart.h"
#include "plic.h"

void external_handler()
{
  //riscv_print("uart_ip:%x\n", *(uint32_t *)UART_IP);
  //riscv_print("uart_ie:%x\n", *(uint32_t *)UART_IE);

  int irq = plic_claim();

  if(irq == UART0_IRQ)
    uart_isr();
  //else
    //riscv_print("unexpected interrupt irq %d\n", irq);

  if (irq) {
    plic_complete(irq);
  }
}

uint32_t trap_handler(uint32_t epc, uint32_t cause)
{
  uint32_t return_pc = epc;
  uint32_t cause_code = cause & 0x7FFFFFFF;

  if(cause & 0x80000000)
  {
    //interrupt
    switch (cause_code) {
      case 3:
        riscv_print("software interrupt\n");
        break;
      case 7:
        riscv_print("timer interrupt\n");
        break;
      case 11:
        //riscv_print("external interrupt\n");
        external_handler();
        break;
    }
    return_pc += 4;
  } else {
    //exception 
  }

  return return_pc;
}

void trap_init()
{
  w_mtvec((uint32_t)trap_vector);
}

void uart_isr()
{
  unsigned char ch = uart_getc();

  if(ch == -1)
    return;
  else
  {
    uart_putc(ch);
    uart_putc('\n');
  }
}

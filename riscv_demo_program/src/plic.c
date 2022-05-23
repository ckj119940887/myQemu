#include "platform.h"
#include "peripheral.h"
#include "plic.h"

void plic_init()
{
  //configure uart irq priority
  //1 is the lowest active priority.
  write_reg((uint32_t *)PLIC_PRIORITY(UART0_IRQ), 1);

  //enable uart irq
  write_reg((uint32_t *)PLIC_MENABLE(UART0_IRQ), 1 << (UART0_IRQ % XLEN));

  //setting priority threshold
  write_reg((uint32_t *)PLIC_MTHRESHOLD, 0);

  //enable machine-mode external interrupt
  w_mie(r_mie() | MIE_MEIE);
  
  //enable machine-mode global interrupt
  w_mstatus(r_mstatus() | MSTATUS_MIE);
}

uint32_t plic_claim()
{
  uint32_t res;
  res = read_reg((uint32_t *)PLIC_CLAIM);
  return res;
}

void plic_complete(uint32_t irq)
{
  write_reg((uint32_t *)PLIC_COMPLETE, irq); 
}

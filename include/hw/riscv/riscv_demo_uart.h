#ifndef __RISCV_DEMO_UART_H_
#define __RISCV_DEMO_UART_H_

#include "chardev/char-fe.h"
#include "qemu/typedefs.h"
#include "qom/object.h"
#include <stdint.h>

enum {
  RISCV_DEMO_UART_TXFIFO = 0,
  RISCV_DEMO_UART_RXFIFO = 4,
  RISCV_DEMO_UART_TXCTRL = 8,
  RISCV_DEMO_UART_TXMARK = 10,
  RISCV_DEMO_UART_RXCTRL = 12,
  RISCV_DEMO_UART_RXMARK = 14,
  RISCV_DEMO_UART_IE     = 16,
  RISCV_DEMO_UART_IP     = 20,
  RISCV_DEMO_UART_DIV    = 24,
  RISCV_DEMO_UART_MAX    = 32
};

enum {
  RISCV_DEMO_UART_IE_TXWM = 1, /*transmit interrupt enable*/
  RISCV_DEMO_UART_IE_RXWM = 2  /*receive interrupt enable*/
};

enum {
  RISCV_DEMO_UART_IP_TXWM = 1, /*transmit interrupt pending*/
  RISCV_DEMO_UART_IP_RXWM = 2  /*receive interrupt pending*/
};

#define RISCV_DEMO_UART_GET_TXCNT(txctrl) ((txctrl >> 16) & 0x7)
#define RISCV_DEMO_UART_GET_RXCNT(rxctrl) ((rxctrl >> 16) & 0x7)
#define RISCV_DEMO_UART_RX_FIFO_SIZE 8

#define TYPE_RISCV_DEMO_UART "riscv.demo.uart"
OBJECT_DECLARE_SIMPLE_TYPE(RISCVDemoUartState, RISCV_DEMO_UART);

typedef struct RISCVDemoUartState
{
  SysBusDevice parent_obj;

  qemu_irq irq;
  MemoryRegion mmio;
  CharBackend chr;
  uint8_t rx_fifo[RISCV_DEMO_UART_RX_FIFO_SIZE];
  uint8_t rx_fifo_len;
  uint32_t ie;
  uint32_t ip;
  uint32_t txctrl;
  uint32_t rxctrl;
  uint32_t div;
}RISCVDemoUartState;

RISCVDemoUartState *riscv_demo_uart_create(MemoryRegion *address_space, hwaddr base, 
    Chardev *chr, qemu_irq irq);

#endif

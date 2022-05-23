#ifndef __RISCV_DEMO_M_H_
#define __RISCV_DEMO_M_H_

#include "hw/riscv/riscv_hart.h"

#define TYPE_RISCV_DEMO_SOC "riscv.demo.soc"
#define RISCV_DEMO_SOC(obj) \
  OBJECT_CHECK(RISCVDemoSoCState, (obj), TYPE_RISCV_DEMO_SOC)

typedef struct RISCVDemoSoCState
{
  /*<private>*/
  DeviceState parent_obj;

  /*<private>*/
  RISCVHartArrayState cpus;
  MemoryRegion xip;
  MemoryRegion maskRom;
  MemoryRegion dram;
  DeviceState *plic;
}RISCVDemoSoCState;

#define TYPE_RISCV_DEMO "riscv.demo"
#define RISCV_DEMO(obj) \
  OBJECT_CHECK(RISCVDemoState, (obj), TYPE_RISCV_DEMO)

typedef struct RISCVDemoState
{
  SysBusDevice parent_obj;

  RISCVDemoSoCState soc;
}RISCVDemoState;


enum {
  RISCV_DEMO_MASKROM,
  RISCV_DEMO_PLIC,
  RISCV_DEMO_UART0,
  RISCV_DEMO_XIP,
  RISCV_DEMO_DRAM,
};

enum {
  RISCV_DEMO_UART0_IRQ = 3,
  RISCV_DEMO_UART1_IRQ = 4,
  RISCV_DEMO_GPIO0_IRQ = 8
};

#define RISCV_DEMO_PLIC_HART_CONFIG "M"
#define RISCV_DEMO_PLIC_NUM_SOURCES 127
#define RISCV_DEMO_PLIC_NUM_PRIORITIES 7
#define RISCV_DEMO_PLIC_PRIORITY_BASE 0x04
#define RISCV_DEMO_PLIC_PENDING_BASE 0x1000
#define RISCV_DEMO_PLIC_ENABLE_BASE 0x2000
#define RISCV_DEMO_PLIC_ENABLE_STRIDE 0x80
#define RISCV_DEMO_PLIC_CONTEXT_BASE 0x200000
#define RISCV_DEMO_PLIC_CONTEXT_STRIDE 0x1000

#endif

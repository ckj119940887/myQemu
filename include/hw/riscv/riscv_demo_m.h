#ifndef __RISCV_DEMO_M_H_
#define __RISCV_DEMO_M_H_

#include "hw/riscv/riscv_hart.h"

#define TYPE_RISCV_DEMO_M_SOC "riscv.demo.m.soc"
#define RISCV_DEMO_M_SOC(obj) \
  OBJECT_CHECK(RISCVDemoMSoCState, (obj), TYPE_RISCV_DEMO_M_SOC)

typedef struct RISCVDemoMSoCState
{
  /*<private>*/
  DeviceState parent_obj;

  /*<private>*/
  RISCVHartArrayState cpus;
  MemoryRegion xip;
  MemoryRegion maskRom;
  MemoryRegion dram;
}RISCVDemoMSoCState;

#define TYPE_RISCV_DEMO_M "riscv.demo.m"
#define RISCV_DEMO_M(obj) \
  OBJECT_CHECK(RISCVDemoMState, (obj), TYPE_RISCV_DEMO_M)

typedef struct RISCVDemoMState
{
  SysBusDevice parent_obj;

  RISCVDemoMSoCState soc;
}RISCVDemoMState;


enum {
  RISCV_DEMO_M_MASKROM,
  RISCV_DEMO_M_UART0,
  RISCV_DEMO_M_XIP,
  RISCV_DEMO_M_DRAM,
};
#endif

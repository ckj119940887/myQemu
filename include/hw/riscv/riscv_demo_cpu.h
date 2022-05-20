#ifndef __RISCV_DEMO_CPU_H_
#define __RISCV_DEMO_CPU_H_

#if defined (TARGET_RISCV32)
#define RISCV_DEMO_M_CPU TYPE_RISCV_DEMO_M_CPU_32
#elif defined (TARGET_RISCV64)
#define RISCV_DEMO_M_CPU TYPE_RISCV_DEMO_M_CPU_64
#endif

#endif


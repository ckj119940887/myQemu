#ifndef __PLATFORM_H_
#define __PLATFORM_H_

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

#include "uart.h"

#define XLEN 32

//MIE
#define MIE_MEIE (1 << 11)

//MSTATUS
#define MSTATUS_MIE (1 << 3)

//static inline functions
//read mie
static inline uint32_t r_mie()
{
  uint32_t res;
  asm volatile ("csrr %0, mie":"=r"(res)::);
  return res;
}

//write mie
static inline void w_mie(uint32_t val)
{
  asm volatile ("csrw mie, %0"::"r"(val):);
}

//read mstatus
static inline uint32_t r_mstatus()
{
  uint32_t res;
  asm volatile ("csrr %0, mstatus":"=r"(res)::);
  return res;
}

//write mstatus
static inline void w_mstatus(uint32_t val)
{
  asm volatile ("csrw mstatus, %0"::"r"(val):);
}

//write mtvec
static inline void w_mtvec(uint32_t val)
{
  asm volatile ("csrw mtvec, %0"::"r"(val):);
}

//printf
int riscv_print(const char *s, ...);

//interuupt handling
void trap_vector(void);
uint32_t trap_handler(uint32_t epc, uint32_t cause);
void trap_init(void);

#endif

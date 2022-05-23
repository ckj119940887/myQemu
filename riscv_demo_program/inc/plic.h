#ifndef __PLIC_H_
#define __PLIC_H_

#define PLIC_BASE 0x0C000000u
#define PLIC_PRIORITY(id) (PLIC_BASE + (id) * 4)
#define PLIC_PENDING(id) (PLIC_BASE + 0x1000 + ((id) >> 5) *4)
#define PLIC_MENABLE(id) (PLIC_BASE + 0x2000 + ((id) >> 5) *4)
#define PLIC_MTHRESHOLD (PLIC_BASE + 0x200000)
#define PLIC_CLAIM (PLIC_BASE + 0x200000 + 0x4)
#define PLIC_COMPLETE (PLIC_BASE + 0x200000 + 0x4)

void plic_init(void);
uint32_t plic_claim(void);
void plic_complete(uint32_t irq);

#endif

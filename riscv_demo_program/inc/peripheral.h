#ifndef __PERIPHERAL_H_
#define __PERIPHERAL_H_

#include <stdint.h>

uint32_t read_reg(uint32_t *address);
void write_reg(uint32_t *address, uint32_t value);

#endif

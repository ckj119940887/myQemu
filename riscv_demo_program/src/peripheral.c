#include "peripheral.h"

uint32_t read_reg(uint32_t *address)
{
  return *address;
}

void write_reg(uint32_t *address, uint32_t value)
{
  *address = value;
}

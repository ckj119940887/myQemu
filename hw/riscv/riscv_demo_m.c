#include "qemu/osdep.h"
#include "qemu/log-for-trace.h"
#include "exec/address-spaces.h"
#include "exec/memory.h"
#include "hw/qdev-core.h"
#include "hw/riscv/boot.h"
#include "hw/boards.h"
#include "hw/riscv/riscv_hart.h"
#include "hw/riscv/riscv_demo_cpu.h"
#include "qapi/error.h"
#include "qemu/bswap.h"
#include "hw/riscv/riscv_demo_m.h"
#include "qemu/typedefs.h"
#include "qom/object.h"
#include <assert.h>
#include <stdint.h>

static const MemMapEntry riscv_demo_m_soc_memmap[] = {
  [RISCV_DEMO_M_MASKROM]  = {       0x0,     0x1000},
  [RISCV_DEMO_M_UART0]     = {0x10000000,     0x1000},
  [RISCV_DEMO_M_XIP]      = {0x20000000, 0x8000000},
  [RISCV_DEMO_M_DRAM]     = {0x80000000, 0x8000000}
};

static void riscv_demo_m_init(MachineState *mc)
{
  const MemMapEntry *memmap = riscv_demo_m_soc_memmap;

  //mc不是一个RISCVDemoMState,因为RISCVDemoMState是SysBusDevice，所以本质上还是Device
  //SysBusDevice的parent是DeviceState,与MachineState完全不是一回事
  //RISCVDemoMState *s = RISCV_DEMO_M(mc);

  RISCVDemoMState *s = g_new0(RISCVDemoMState, 1);
  MemoryRegion *sysmem = get_system_memory();

  assert(s != NULL);

  //initialize SoC
  object_initialize_child(OBJECT(mc), "soc", &s->soc, TYPE_RISCV_DEMO_M_SOC);
  qdev_realize(DEVICE(&s->soc), NULL, &error_abort);

  //dram
  memory_region_init_ram(&s->soc.dram, NULL, "riscv.demo.m.dram", \
      memmap[RISCV_DEMO_M_DRAM].size, &error_abort);
  memory_region_add_subregion(sysmem, memmap[RISCV_DEMO_M_DRAM].base, &s->soc.dram);
  
  //resetvec
  uint32_t reset_vec[4];

  reset_vec[0] = 0x200002b7; //0x0000, lui t0, 0x20000
  reset_vec[1] = 0x00028067; //0x0004, jr t0
  reset_vec[2] = reset_vec[3] = 0;

  for (int i = 0; i < sizeof(reset_vec) >> 2; i++)
    reset_vec[i] = cpu_to_le32(reset_vec[i]);
  
  rom_add_blob_fixed_as("maskRom.reset", reset_vec, sizeof(reset_vec), \
      memmap[RISCV_DEMO_M_MASKROM].base, &address_space_memory);
  
  if(mc->kernel_filename) {
    riscv_load_kernel(mc->kernel_filename, memmap[RISCV_DEMO_M_MASKROM].base, NULL);
  }
  
}

static void riscv_demo_m_class_init(ObjectClass *klass, void *data)
{
  MachineClass *mc = MACHINE_CLASS(klass);

  mc->desc = "RISC-V demo board, only support Machine mode";
  mc->init = riscv_demo_m_init;
  mc->max_cpus = 1;
  //该值必须要进行赋值
  //与该值相关的文件有include/hw/riscv/riscv_deom_cpu.h
  //target/riscv/cpu.h, target/riscv/cpu.c
  mc->default_cpu_type = RISCV_DEMO_M_CPU;
}

static const TypeInfo riscv_demo_m_type_info = {
  .name = MACHINE_TYPE_NAME("riscv_demo_m"),
  .parent = TYPE_MACHINE,
  .class_init = riscv_demo_m_class_init,
  .instance_size = sizeof(RISCVDemoMState),
};

static void riscv_demo_m_register_types(void)
{
  type_register_static(&riscv_demo_m_type_info);
}

type_init(riscv_demo_m_register_types)

////////////////////////////////////////////////////////////////////

static void riscv_demo_m_soc_instance_init(Object *obj)
{
  MachineState *ms = MACHINE(qdev_get_machine());
  RISCVDemoMSoCState *s = RISCV_DEMO_M_SOC(obj);

  object_initialize_child(obj, "cpus", &s->cpus, TYPE_RISCV_HART_ARRAY);
  object_property_set_int(OBJECT(&s->cpus), "num-harts", ms->smp.cpus, &error_abort);
  object_property_set_int(OBJECT(&s->cpus), "resetvec", 0x1004, &error_abort);
}

static void riscv_demo_m_soc_realize(DeviceState *dev, Error **errp)
{
  MachineState *ms = MACHINE(qdev_get_machine());
  const MemMapEntry *memmap = riscv_demo_m_soc_memmap;
  RISCVDemoMSoCState *s = RISCV_DEMO_M_SOC(dev);
  MemoryRegion *sysmem = get_system_memory();

  //标记CPU已经被realized
  object_property_set_str(OBJECT(&s->cpus), "cpu-type", ms->cpu_type, &error_abort);
  sysbus_realize(SYS_BUS_DEVICE(&s->cpus), &error_abort);

  //注册maskrom
  memory_region_init_rom(&s->maskRom, OBJECT(dev), "riscv.demo.m.maskrom", \
      memmap[RISCV_DEMO_M_MASKROM].size, &error_fatal);
  memory_region_add_subregion(sysmem, memmap[RISCV_DEMO_M_MASKROM].base, &s->maskRom);

  //注册xip
  memory_region_init_rom(&s->xip, OBJECT(dev), "riscv.demo.m.xip", \
      memmap[RISCV_DEMO_M_XIP].size, &error_fatal);
  memory_region_add_subregion(sysmem, memmap[RISCV_DEMO_M_XIP].base, &s->xip);

}

static void riscv_demo_m_soc_class_init(ObjectClass *klass, void *data)
{
  DeviceClass *dc = DEVICE_CLASS(klass);
  dc->realize = riscv_demo_m_soc_realize;
  dc->user_creatable = false;
}

static const TypeInfo riscv_demo_m_soc_type_info = {
  .name = TYPE_RISCV_DEMO_M_SOC,
  .parent = TYPE_DEVICE,
  .instance_size = sizeof(RISCVDemoMSoCState),
  .instance_init = riscv_demo_m_soc_instance_init,
  .class_init = riscv_demo_m_soc_class_init,
};

static void riscv_demo_m_soc_register_types(void)
{
  type_register_static(&riscv_demo_m_soc_type_info);
}

type_init(riscv_demo_m_soc_register_types)

#include "qemu/osdep.h"
#include "hw/qdev-properties-system.h"
#include "qapi/error.h"
#include "exec/cpu-common.h"
#include "chardev/char-fe.h"
#include "chardev/char.h"
#include "exec/memory.h"
#include "hw/sysbus.h"
#include "exec/hwaddr.h"
#include "qom/object.h"
#include "qemu/module.h"
#include "hw/qdev-core.h"
#include "hw/irq.h"
#include "hw/riscv/riscv_demo_uart.h"
#include "migration/vmstate.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint64_t riscv_demo_uart_ip(RISCVDemoUartState *s)
{
  uint64_t ret = 0;
  
  uint64_t txcnt = RISCV_DEMO_UART_GET_TXCNT(s->txctrl);
  uint64_t rxcnt = RISCV_DEMO_UART_GET_RXCNT(s->rxctrl);

  if(txcnt != 0)
  {
    ret |= RISCV_DEMO_UART_IP_TXWM; 
  }

  if(s->rx_fifo_len > rxcnt)
  {
    ret |= RISCV_DEMO_UART_IP_RXWM;
  }

  return ret;
}

static void riscv_demo_uart_update_irq(RISCVDemoUartState *s)
{
  //问题代码  
  /*
  int cond = 0;
  if((s->ie & RISCV_DEMO_UART_IE_TXWM) ||
     ((s->ie & RISCV_DEMO_UART_IE_RXWM) && (s->rx_fifo_len > 0)))
  {
    cond = 1;
  }

  if(cond == 1)
  {
    qemu_irq_raise(s->irq);
  }
  else
  {
    qemu_irq_lower(s->irq);
  }
  */
  
  if((s->ie & RISCV_DEMO_UART_IP_RXWM) && (s->rx_fifo_len > 0))
  {
    //printf("raise irq\n");
    qemu_irq_raise(s->irq);
  } else {
    qemu_irq_lower(s->irq);
  }
}

static uint64_t 
riscv_demo_uart_mem_read(void *opaque, hwaddr addr, unsigned int size)
{
  RISCVDemoUartState *s = opaque;
  unsigned char r;

  switch (addr) {
    case RISCV_DEMO_UART_RXFIFO:
      if(s->rx_fifo_len) {
        /*
        printf("rx_fifo:\t");
        for(int i = 0; i < s->rx_fifo_len; i++)
          printf("%c", s->rx_fifo[i]);
        printf("\n");
        */

        r = s->rx_fifo[0];
        memmove(s->rx_fifo, s->rx_fifo + 1, s->rx_fifo_len - 1);
        s->rx_fifo_len --;
        qemu_chr_fe_accept_input(&s->chr);
        riscv_demo_uart_update_irq(s);

        return r;
      }
      return 0x80000000;
    case RISCV_DEMO_UART_TXFIFO:
      return 0;
    case RISCV_DEMO_UART_IE:
      return s->ie;
    case RISCV_DEMO_UART_IP:
      return riscv_demo_uart_ip(s);
    case RISCV_DEMO_UART_RXCTRL:
      return s->rxctrl;
    case RISCV_DEMO_UART_TXCTRL:
      return s->txctrl;
    case RISCV_DEMO_UART_DIV:
      return s->div;
  }

  return 0;
}

static void 
riscv_demo_uart_mem_write(void *opaque, hwaddr addr, uint64_t val64, unsigned int size)
{
  RISCVDemoUartState *s = opaque;
  uint32_t value = val64;
  unsigned char ch = value;

  switch (addr) {
    case RISCV_DEMO_UART_TXFIFO:
      qemu_chr_fe_write(&s->chr, &ch, 1);
      riscv_demo_uart_update_irq(s);
      return;
    case RISCV_DEMO_UART_IE:
      s->ie = val64;
      riscv_demo_uart_update_irq(s);
      return;
    case RISCV_DEMO_UART_TXCTRL:
      s->txctrl = val64;
      return;
    case RISCV_DEMO_UART_RXCTRL:
      s->rxctrl = val64;
      return;
    case RISCV_DEMO_UART_DIV:
      s->div = val64;
      return;
  }
}

static const MemoryRegionOps riscv_demo_uart_memops = {
  .read = riscv_demo_uart_mem_read,
  .write = riscv_demo_uart_mem_write,
  .endianness = DEVICE_LITTLE_ENDIAN,
  .valid = {
    .min_access_size = 4,
    .max_access_size = 4
  }
};

static void riscv_demo_uart_instance_init(Object *obj)
{
  SysBusDevice *sbd = SYS_BUS_DEVICE(obj);  
  RISCVDemoUartState *s = RISCV_DEMO_UART(obj);

  memory_region_init_io(&s->mmio, OBJECT(s), &riscv_demo_uart_memops, s, \
      TYPE_RISCV_DEMO_UART, RISCV_DEMO_UART_MAX);
  sysbus_init_mmio(sbd, &s->mmio);
  sysbus_init_irq(sbd, &s->irq);
}

static void riscv_demo_uart_event(void *opaque, QEMUChrEvent event)
{
}

static void riscv_demo_uart_rx(void *opaque, const uint8_t *buf, int size)
{
  RISCVDemoUartState *s = opaque;
  if (s->rx_fifo_len >= sizeof(s->rx_fifo)) {
    printf("UART dropped char.\n");
    return;
  }
  s->rx_fifo[s->rx_fifo_len++] = *buf;

  riscv_demo_uart_update_irq(s);
}

static int riscv_demo_uart_can_rx(void *opaque) 
{
  RISCVDemoUartState *s = opaque;
  return s->rx_fifo_len < sizeof(s->rx_fifo);
}

static int riscv_demo_uart_be_change(void *opaque)
{
  RISCVDemoUartState *s = opaque;

  qemu_chr_fe_set_handlers(&s->chr, riscv_demo_uart_can_rx, riscv_demo_uart_rx, \
      riscv_demo_uart_event, riscv_demo_uart_be_change, s, NULL, true);

  return 0;
}

static void riscv_demo_uart_realize(DeviceState *dev, Error **errp)
{
  RISCVDemoUartState *s = RISCV_DEMO_UART(dev);

  qemu_chr_fe_set_handlers(&s->chr, riscv_demo_uart_can_rx, riscv_demo_uart_rx, \
      riscv_demo_uart_event, riscv_demo_uart_be_change, s, NULL, true);
}

static Property riscv_demo_uart_properties[] = {
  DEFINE_PROP_CHR("chardev", RISCVDemoUartState, chr),
  DEFINE_PROP_END_OF_LIST(),
};

static void riscv_demo_uart_enter(Object *obj, ResetType type)
{
  RISCVDemoUartState *s = RISCV_DEMO_UART(obj);
  s->ie = 0;
  s->ip = 0;
  s->txctrl = 0;
  s->rxctrl = 0;
  s->div = 0;
  s->rx_fifo_len = 0;
}

static void riscv_demo_uart_hold(Object *obj)
{
  RISCVDemoUartState *s = RISCV_DEMO_UART(obj);
  qemu_irq_lower(s->irq);
}

static const VMStateDescription vmstate_riscv_demo_uart = {
  .name = TYPE_RISCV_DEMO_UART,
  .version_id = 1,
  .minimum_version_id = 1,
  .fields = (VMStateField[]) {
    VMSTATE_UINT8_ARRAY(rx_fifo, RISCVDemoUartState, RISCV_DEMO_UART_RX_FIFO_SIZE),
    VMSTATE_UINT8(rx_fifo_len, RISCVDemoUartState),
    VMSTATE_UINT32(ie, RISCVDemoUartState),
    VMSTATE_UINT32(ip, RISCVDemoUartState),
    VMSTATE_UINT32(txctrl, RISCVDemoUartState),
    VMSTATE_UINT32(rxctrl, RISCVDemoUartState),
    VMSTATE_UINT32(div, RISCVDemoUartState),
    VMSTATE_END_OF_LIST()
  },
};

static void riscv_demo_uart_class_init(ObjectClass *oc, void *data)
{
  DeviceClass *dc = DEVICE_CLASS(oc);
  ResettableClass *rc = RESETTABLE_CLASS(oc);

  dc->realize = riscv_demo_uart_realize;
  dc->vmsd = &vmstate_riscv_demo_uart;
  rc->phases.enter = riscv_demo_uart_enter;
  rc->phases.hold = riscv_demo_uart_hold;
  device_class_set_props(dc, riscv_demo_uart_properties);
}

static const TypeInfo riscv_demo_uart_type_info = {
  .name = TYPE_RISCV_DEMO_UART,
  .parent = TYPE_SYS_BUS_DEVICE,
  .instance_size = sizeof(RISCVDemoUartState),
  .instance_init = riscv_demo_uart_instance_init,
  .class_init = riscv_demo_uart_class_init,
};

static void riscv_demo_uart_register_types(void)
{
  type_register_static(&riscv_demo_uart_type_info);
}

type_init(riscv_demo_uart_register_types)

RISCVDemoUartState *riscv_demo_uart_create(MemoryRegion *address_space, hwaddr base,
    Chardev *chr, qemu_irq irq)
{
  DeviceState *dev;
  SysBusDevice *sbd;
  RISCVDemoUartState *s;

  dev = qdev_new(TYPE_RISCV_DEMO_UART);
  sbd = SYS_BUS_DEVICE(dev);
  qdev_prop_set_chr(dev, "chardev", chr);
  sysbus_realize_and_unref(sbd, &error_fatal);
  memory_region_add_subregion(address_space, base, sysbus_mmio_get_region(sbd, 0));
  sysbus_connect_irq(sbd, 0, irq);
  s = RISCV_DEMO_UART(dev);

  return s;
}

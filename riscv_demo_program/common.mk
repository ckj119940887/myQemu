.PHONY:all debug clean gdb run 

CROSS_COMPILE = riscv64-unknown-elf-
QEMU = ../build/qemu-system-riscv32 
QFLAGS = -nographic -smp 1 -M riscv_demo_m -cpu riscv_demo_m_32 -bios none

CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
GDB = $(CROSS_COMPILE)gdb
CFLAGS = -nostdlib -fno-builtin -march=rv32ima -mabi=ilp32 -g -Wall -D CONFIG_SYSCALL

SRCS = $(wildcard *.c)
ASMS = $(wildcard *.s) $(wildcard *.S)   
SRC_OBJ = $(SRCS:%.c=%.o) 
ASM_OBJ = $(patsubst %.S,%.o,$(patsubst %.s,%.o,$(ASMS)))
DEPS = $(SRCS:%.c=%.d) 

OBJS_DIR = $(BUILD_ROOT)/objs_dir
DEPS_DIR = $(BUILD_ROOT)/deps_dir
INC_DIR = $(BUILD_ROOT)/inc
TARGET_DIR = $(BUILD_ROOT)/target

TARGET := $(TARGET_DIR)/$(TARGET) 

$(shell mkdir -p $(OBJS_DIR))
$(shell mkdir -p $(DEPS_DIR))
$(shell mkdir -p $(TARGET_DIR))

SRC_OBJ := $(addprefix $(OBJS_DIR)/,$(SRC_OBJ))
ASM_OBJ := $(addprefix $(OBJS_DIR)/,$(ASM_OBJ))
DEPS := $(addprefix $(DEPS_DIR)/,$(DEPS))

all: $(SRC_OBJ) $(ASM_OBJ) $(DEPS) $(TARGET) 

ifneq ($(wildcard $(DEPS_DIR)/*.d),"")
include $(DEPS)
endif

$(TARGET): $(ASM_OBJ) $(SRC_OBJ)   
	$(CC) -T $(BUILD_ROOT)/linker_script $(CFLAGS) -o $(TARGET) $^
	$(OBJCOPY) -O binary $(OBJFLAGS) $(TARGET) $(addsuffix .bin,$(basename $(TARGET)))

$(OBJS_DIR)/%.o:%.c
	$(CC) -I$(INC_DIR) -o $@ $(CFLAGS) -c $^

$(OBJS_DIR)/%.o:%.s
	$(CC) -I$(INC_DIR) $(CFLAGS) -o $@ -c $^

$(OBJS_DIR)/%.o:%.S
	$(CC) -I$(INC_DIR) $(CFLAGS) -o $@ -c $^

$(DEPS_DIR)/%.d:%.c
	$(CC) -I$(INC_DIR) -MM $^ > $@

run: 
	$(QEMU) $(QFLAGS) -kernel $(TARGET_DIR)/$(TARGET)

debug:
	$(QEMU) $(QFLAGS) -kernel $(TARGET_DIR)/$(TARGET) -s -S

gdb:
	$(GDB) $(TARGET_DIR)/$(TARGET) -q -x gdbinit

clean:
	@echo "DEPS=$(DEPS)"
	rm -rf *.elf *.bin  
	rm -rf $(ASM_OBJ) $(SRC_OBJ)
	rm -rf $(OBJS_DIR) $(DEPS_DIR)
	rm -rf *.uimg *.img 

export BUILD_ROOT = $(shell pwd)

.PHONY: all clean debug gdb run

all:
	make -C src

debug:
	make -f common.mk TARGET=start.elf debug

gdb:
	make -f common.mk TARGET=start.elf gdb

run:
	make -f common.mk TARGET=start.elf run

clean:
	rm -rf objs_dir deps_dir
	rm -rf *.elf *.bin *.uimg *.img
	rm -rf target

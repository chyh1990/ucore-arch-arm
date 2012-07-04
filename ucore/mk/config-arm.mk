#export ARCH_ARM_CPU := arm926ej-s
export ARCH_ARM_MACH := armv5
export ARCH_ARM_BOOTLOADER_BASE :=0x72f00000

export ARCH_ARM_BOARD :=versatilepb
export ARCH_ARM_KERNEL_BASE :=0x00008000  #versatilepb

#export ARCH_ARM_BOARD :=at91
#export ARCH_ARM_KERNEL_BASE :=0x70008000   #at91 board

export PLATFORM_DEF := -DPLATFORM_$(shell echo $(ARCH_ARM_BOARD) | tr 'a-z' 'A-Z') -DCONFIG_NO_SWAP

export TARGET_CC_SYSTEM_LIB ?=  -L/opt/FriendlyARM/toolschain/4.4.3/lib/gcc/arm-none-linux-gnueabi/4.4.3/ 

export HOST_CC_PREFIX	?=
export TARGET_CC_PREFIX	?= arm-linux-
export TARGET_CC_FLAGS_COMMON	?= -fno-builtin -nostdinc -fno-stack-protector -nostartfiles -march=$(ARCH_ARM_MACH) $(PLATFORM_DEF) -DARCH_ARM
export TARGET_CC_FLAGS_BL		?=
export TARGET_CC_FLAGS_KERNEL	?=
export TARGET_CC_FLAGS_SV		?=
export TARGET_CC_FLAGS_USER		?=
export TARGET_LD_FLAGS			?= -nostdlib $(TARGET_CC_SYSTEM_LIB) 

export KERNEL_FILENAME := kernel-$(ARCH_ARM_BOARD)
export KERNEL_INITRD_FILENAME := kernel-initrd-$(ARCH_ARM_BOARD)
export BOOTLOADER_FILENAME := bootloader-$(ARCH_ARM_BOARD)


TERMINAL	?= gnome-terminal
SIMULATOR	?= qemu-system-arm 
RAMIMG		:= ${T_OBJ}/ram.img
GDB       := arm-linux-gdb

MKIMAGE ?= misc/mkimage
OBJCOPY := $(TARGET_CC_PREFIX)objcopy
OBJDUMP := $(TARGET_CC_PREFIX)objdump

run: all
	@echo "run NOT impl"


.PHONY: sim
sim: $(T_OBJ)/$(KERNEL_INITRD_FILENAME)
	$(SIMULATOR) -kernel $(T_OBJ)/$(KERNEL_INITRD_FILENAME)  -M versatilepb -m 64M -serial stdio

.PHONY: debug
debug: $(T_OBJ)/$(KERNEL_INITRD_FILENAME)
	$(TERMINAL) -e "$(SIMULATOR) -kernel $(T_OBJ)/$(KERNEL_INITRD_FILENAME)  -S -s -M versatilepb -m 64M -serial stdio"
	sleep 1
	$(GDB) -x gdbinit.arm



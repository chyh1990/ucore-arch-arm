export ARCH_ARM_CPU := arm926ej-s
export ARCH_ARM_BOARD := at91
export ARCH_ARM_BOOTLOADER_BASE :=0x73f00000
export ARCH_ARM_KERNEL_BASE :=0x70100000
export PLATFORM_DEF := -DPLATFORM_$(shell echo $(ARCH_ARM_BOARD) | tr 'a-z' 'A-Z') 

export TARGET_CC_SYSTEM_LIB ?=  -L/opt/FriendlyARM/toolschain/4.4.3/lib/gcc/arm-none-linux-gnueabi/4.4.3/ 

export HOST_CC_PREFIX	?=
export TARGET_CC_PREFIX	?= arm-linux-
export TARGET_CC_FLAGS_COMMON	?= -fno-builtin -nostdinc -fno-stack-protector -nostartfiles -mcpu=$(ARCH_ARM_CPU) $(PLATFORM_DEF) 
export TARGET_CC_FLAGS_BL		?=
export TARGET_CC_FLAGS_KERNEL	?=
export TARGET_CC_FLAGS_SV		?=
export TARGET_CC_FLAGS_USER		?=
export TARGET_LD_FLAGS			?= -nostdlib $(TARGET_CC_SYSTEM_LIB) 

export KERNEL_FILENAME := kernel-$(ARCH_ARM_BOARD)
export BOOTLOADER_FILENAME := bootloader-$(ARCH_ARM_BOARD)


TERMINAL	?= gnome-terminal
SIMULATOR	?= qemu-system-arm 
RAMIMG		:= ${T_OBJ}/ram.img

MKIMAGE ?= misc/mkimage
OBJCOPY := $(TARGET_CC_PREFIX)objcopy
OBJDUMP := $(TARGET_CC_PREFIX)objdump

run: all
	@echo "run NOT impl"

debug: all
	@echo "debug NOT impl"
	

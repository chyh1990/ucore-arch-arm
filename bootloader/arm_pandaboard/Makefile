BOARD_NAME:= panda
TARGET:=elfloader-$(BOARD_NAME).bin
CROSS_COMPILE:=arm-linux-
LD:=$(CROSS_COMPILE)ld
CC:=$(CROSS_COMPILE)gcc
OBJCOPY:=$(CROSS_COMPILE)objcopy

BOOTLOADER_BASE:=0x90000000
CFLAGS:= -nostdinc -nostdlib -fno-builtin -DBOOTLOADER_BASE=$(BOOTLOADER_BASE) -march=armv5

OBJS := bootasm.o bootmain.o eabi_utils.o memcpy.o

all: $(TARGET) boot.scr kernel.img

KERNEL_ELF := ../../ucore/obj/kernel/kernel-arm.elf
kernel.img: $(TARGET) $(KERNEL_ELF)
	rm -f $@
	dd if=$(TARGET) of=$@
	dd if=$(KERNEL_ELF) of=$@ seek=8 conv=notrunc

$(TARGET).elf: $(OBJS) bootloader.ld
	$(LD) -T bootloader.ld -o $@ $(OBJS)

$(TARGET): $(TARGET).elf
	$(OBJCOPY) -S -O binary $< $@

%.o: %.S
	$(CC) -c $(CFLAGS) -o $@ $<

bootloader.ld : bootloader.ld.in
	sed 's/TEXT_BASE/$(BOOTLOADER_BASE)/' $< > $@	

%.scr: %.cmd
	mkimage -A arm -O linux -T script -C none -a 0 -e 0 -n "bootscr" -d $< $@

clean:
	rm -f bootloader.ld *.scr
	rm -f *.o $(TARGET) $(TARGET).elf

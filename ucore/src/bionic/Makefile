
CROSS_COMPILE ?= arm-linux-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
AR := $(CROSS_COMPILE)ar
OBJCOPY := $(CROSS_COMPILE)objcopy
TARGET_CC := $(CROSS_COMPILE)gcc

CFLAGS += -I./libc/include
CFLAGS += -I./libc/arch-arm/include
CFLAGS += -I./libc/kernel/common
CFLAGS += -I./libm/include
CFLAGS += -fno-builtin -nostdinc -nostdlib -fno-stack-protector -nostartfiles
GCCSYSTEM_DIR :=/home/chenyh/android/android_source/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/lib/gcc/arm-eabi/4.4.3
CFLAGS += -isystem $(GCCSYSTEM_DIR)/include

CFLAGS += -march=armv5

test1: test1.c
	$(CC) $(CFLAGS)  -c $<
	$(CC) -Wl,-T user.ld -nostdlib -o $@  ./libc/build/crtbegin_static.o test1.o -L./libc/build  ./libc/build/crtend_android.o  -lc_nomalloc -lc_common  $(GCCSYSTEM_DIR)/libgcc.a

all: test1

clean:
	rm -rf test1 *.o

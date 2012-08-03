LOCAL_PATH := $(shell pwd)
TARGET_ARCH:=arm
LIBC_TYPE:=static

CROSS_COMPILE ?= arm-linux-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
AR := $(CROSS_COMPILE)ar
OBJCOPY := $(CROSS_COMPILE)objcopy
TARGET_CC := $(CROSS_COMPILE)gcc

CFLAGS += -fno-builtin -nostdinc -fno-stack-protector -nostartfiles
CFLAGS += -march=armv5
#CFLAGS += -D__ARM_ARCH__=5

BINDIR:=build
TARGET_OUT_STATIC_LIBRARIES := $(BINDIR)
TARGET_NAME:=$(BINDIR)/libc_$(LIBC_TYPE).a

.PHONY: all clean mklink

all: $(OBJDIR) $(TARGET_NAME)

clean:
	rm -f $(TARGET_NAME)
	find . -name '*.o' -type f -print -exec rm -f {} \;

include Android.mk

OBJS := $(patsubst %.c, %.o, $(LOCAL_SRC_FILES))
OBJS := $(patsubst %.S, %.o, $(OBJS))
OBJS := $(patsubst %.c.$(TARGET_ARCH), %.o, $(OBJS))
#OBJS := $(patsubst %.c.$(TARGET_ARCH), %.o, $(OBJS))

CFLAGS += $(LOCAL_CFLAGS)
CFLAGS += $(addprefix -I,$(LOCAL_C_INCLUDES))
CFLAGS += -I$(LOCAL_PATH)/arch-$(TARGET_ARCH)/include
CFLAGS += -I$(LOCAL_PATH)/include
CFLAGS += -isystem /home/chenyh/android/android_source/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/lib/gcc/arm-eabi/4.4.3/include
CFLAGS += -I$(LOCAL_PATH)/kernel/common
CFLAGS += -I$(LOCAL_PATH)/../libm/include
#CFLAGS += -I/home/chenyh/os/ucore-arch-arm/ucore/src/kern-ucore/module/include

$(BINDIR):
	-mkdir $(BINDIR)

$(TARGET_NAME): mklink $(BINDIR) $(ALL_GENERATED_SOURCES) $(OBJS)
	$(AR) rcs $@  $(OBJS) 

mklink:
	rm -f $(LOCAL_PATH)/include/asm
	ln -s $(LOCAL_PATH)/kernel/arch-$(TARGET_ARCH)/asm $(LOCAL_PATH)/include/

%.o: %.S
	$(CC) $(CFLAGS) -D__ASSEMBLY__ -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o: %.c.$(TARGET_ARCH)
	$(CC) $(CFLAGS) -o $@ -c $<

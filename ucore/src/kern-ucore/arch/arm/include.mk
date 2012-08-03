ARCH_INLUCDES:=debug driver include libs mm process sync trap syscall

ifdef UCONFIG_ARM_BOARD_GOLDFISH
ARCH_INLUCDES += mach-goldfish
PLATFORM_DEF := -DPLATFORM_GOLDFISH

ifdef UCONFIG_HAVE_LINUX_DDE_BASE
PLATFORM_DEF += -include $(KTREE)/module/include/mach-goldfish/autoconf.h
endif

endif


ifdef UCONFIG_ARM_CPU_V7
MACH_MACRO := -D__MACH_ARM_ARMV7 	-D__LINUX_ARM_ARCH__=7
PLATFORM_DEF += -march=armv7-a
endif

MACH_MACRO += -DDEBUG -D__ARM_EABI__ -DCONFIG_NO_SWAP -DARCH_ARM

ARCH_CFLAGS := $(MACH_MACRO) $(PLATFORM_DEF)

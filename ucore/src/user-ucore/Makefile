E_ENCODE = $(shell echo $(1) | sed -e 's!_!_1!g' -e 's!/!_2!g')
E_DECODE = $(shell echo $(1) | sed -e 's!_1!_!g' -e 's!_2!/!g')

.PHONY: all

SRCFILES   := $(filter %.c, $(wildcard *))
T_CC_FLAGS := 	-I../libs-user-ucore/ -I../libs-user-ucore/common -I../libs-user-ucore/arch/${ARCH}

TARGET_CC_SYSTEM_LIB ?=  -L/opt/FriendlyARM/toolschain/4.4.3/lib/gcc/arm-none-linux-gnueabi/4.4.3/ 

include ${T_BASE}/mk/compu.mk
include ${T_BASE}/mk/template.mk

BINFILES := $(addprefix ${T_OBJ}/user-sfs/bin/,${SRCFILES:.c=})

all: ${T_OBJ}/user-sfs ${BINFILES}
	${V}cp -r _initial/* ${T_OBJ}/user-sfs/

${T_OBJ}/user-sfs/bin/%: ${T_OBJ}/user-ucore-$(call E_ENCODE,%).c.o
	@echo LD $@
#	${V}${LD} -T ../libs-user-ucore/arch/${ARCH}/user.ld $< ${T_OBJ}/libs-user-ucore-*.o -o$@
	${V}${LD} $(TARGET_CC_SYSTEM_LIB) -T ../libs-user-ucore/arch/${ARCH}/user.ld $< ${T_OBJ}/ulib.a -lgcc -o$@
	@touch ${T_OBJ}/user-sfs-timestamp

${T_OBJ}/user-sfs:
	${V}mkdir -p $@
	${V}mkdir -p $@/bin

SRCFILES	+= $(shell find arch/${ARCH} '(' '!' -regex '.*/_.*' ')' -and '(' -iname "*.c" -or -iname "*.S" ')' | sed -e 's!\./!!g')
ARCH_DIRS	:= mm driver init libs sync process glue-ucore glue-ucore/libs syscall
T_CC_FLAGS	+= ${foreach dir,${ARCH_DIRS},-Iarch/${ARCH}/${dir}}

LINK_FILE	:= arch/${ARCH}/ucore.ld

include ${T_BASE}/mk/compk.mk
include ${T_BASE}/mk/template.mk

all: ${T_OBJ}/kernel

${T_OBJ}/kernel: ${OBJFILES}
	@echo LD $@
	${V}${LD} -T ${LINK_FILE} -o$@ $+

.PHONY: all
SRCFILES   := $(shell find . '(' '!' -regex '.*/_.*' ')' -and '(' -iname "*.c" -or -iname "*.S" ')' -and -not -regex '\./arch.*' | sed -e 's!\./!!g')
T_CC_FLAGS := -D__KERNEL__ -D__KERN__ -I../glue-kern/arch/${ARCH} -Iglue-ucore -Iglue-ucore/libs \
			  -Ilibs -Isyscall -Idebug -Idriver -Imm -Isync -Ischedule -Iprocess -Ifs \
			  -Ifs/swap -Ifs/vfs -Ifs/pipe -Ifs/sfs -Ifs/devs -Imodule -Imodule/include  -include linux/autoconf.h

include arch/$(ARCH)/ucore.mk


ARCH_DIRS	:= mm driver init sync process trap syscall debug include libs mach-$(ARCH_ARM_BOARD)
SRCFILES	+= $(foreach dir,${ARCH_DIRS}, $(shell find arch/${ARCH}/${dir} '(' '!' -regex '.*/_.*' ')' -and '(' -iname "*.c" -or -iname "*.S" ')' | sed -e 's!\./!!g'))

T_CC_FLAGS	+= ${foreach dir,${ARCH_DIRS},-Iarch/${ARCH}/${dir}} 

LINK_FILE_IN	:= arch/${ARCH}/ucore.ld.in
LINK_FILE     := $(T_OBJ)/ucore-$(ARCH_ARM_BOARD).ld
SEDFLAGS	= s/TEXT_START/$(ARCH_ARM_KERNEL_BASE)/

include ${T_BASE}/mk/compk.mk
include ${T_BASE}/mk/template.mk

all: ${T_OBJ}/$(KERNEL_INITRD_FILENAME)

$(LINK_FILE): $(LINK_FILE_IN)
	@echo "creating linker script"
	@sed  "$(SEDFLAGS)" < $< > $@

${T_OBJ}/$(KERNEL_FILENAME): ${OBJFILES} $(LINK_FILE)
	@echo LD $@
	${V}${LD} -T ${LINK_FILE} -o$@ $+ -lgcc

${T_OBJ}/initrd.o: $(DATA_FILE)
	@echo "Linking Initrd into kernel, using $(DATA_FILE)"
	@sed "s#DATA_FILE#$(DATA_FILE)#" < arch/$(ARCH)/tools/initrd.S.in > $(T_OBJ)/initrd.S
	$(CC) -c $(T_OBJ)/initrd.S -o $(T_OBJ)/initrd.o


${T_OBJ}/$(KERNEL_INITRD_FILENAME): ${OBJFILES} $(LINK_FILE) $(T_OBJ)/initrd.o
	@echo LD $@
	${V}${LD} -T ${LINK_FILE} -o$@ $+ -lgcc


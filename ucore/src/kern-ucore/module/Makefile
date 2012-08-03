obj-y := dde_main.o dev2devfs_glue.o dev2devfs_glue_helper.o fcntl_glue.o kmalloc_glue.o module_exporter.o module_loader.o\
		pmm_glue.o resource_glue.o schedule_glue.o timer_glue.o uaccess_glue.o ucore_stat2linux_stat.o irq_glue.o

dirs-y := drivers fs kernel lib include

dirs-$(UCONFIG_ARM_BOARD_GOLDFISH) += goldfishdrv

obj-$(UCONFIG_ARM_BOARD_GOLDFISH) += arm_iomap_glue.o

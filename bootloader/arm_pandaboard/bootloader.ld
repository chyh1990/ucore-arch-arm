/*
 * bootlink.lds
 * 
 * author: YU Kuanlong / HE Alexis
 */

OUTPUT_ARCH(arm)
ENTRY(_start)

SECTIONS
{
	. = 0x90000000;
	.text : 
	{
		*(.startup)
		*(.text) 
		*(.rodata*)
	}

	. = ALIGN(32);

	.data : 
	{
		*(.data)
	}
	
	. = ALIGN(32);
	
	__bss_start__ = .;
	.bss : 
	{
		*(.bss)
	}
	__bss_end__ = .;
}

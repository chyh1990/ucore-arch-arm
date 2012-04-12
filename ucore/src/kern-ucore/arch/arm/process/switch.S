.text
.globl switch_to
switch_to:                      # switch_to(from, to)
	# TODO: select relevant registers to keep

    # save from's registers
	# in the structure pointed by r0
	stmia r0!, {lr}
	stmia r0!, {sp}
	stmia r0!, {r2-r4, fp, ip}

    # restore to's registers
	# in the structure pointed by r1
	ldmia r1!, {lr}
	ldmia r1!, {sp}
	ldmia r1!, {r2-r4, fp, ip}
	
	# return
	mov pc, lr


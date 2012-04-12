		.text
		.global _start

		// This function should look like this in C:
		//     void _start (int argc, char* argv[])
_start:		
    mov r0, r3
		bl	umain
		nop

		// should never reach here
_dead:
		b _dead
		nop

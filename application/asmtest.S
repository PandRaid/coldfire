/*
This is another test module for the platform
Instead of running an actual application the
paltform runs the coldfire assembly.
There are two makefiles necesarry for this as 
well as the m68k GNU cross compiler plugin
**/

.global _start
_start:	addi.l     #64, %d2
		and.l 	   %d1, %d0
		sub.l 	   %d3, %d4
		eor.l      %d5, %d6
		not.l 	   %d5
		eori.l     #0xFF, %d7
		adda.l     %d3, %a0
		suba.l     %d3, %a1
		andi.l 	   #129, %d2
		add.l      %d1, %d2
		jsr 	   test
		addi.l     #0xFFFFFFFF, %d2
		bcc			exit
		jmp			exit

test:
		add.l      %d1, %d0
		rts
.global exit
exit:
		add.l      %d1, %d0

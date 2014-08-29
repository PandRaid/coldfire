/*
 *
 * Copyright (c) 2005-2014 Imperas Software Ltd., www.imperas.com
 *
 * The contents of this file are provided under the Software License
 * Agreement that you accepted before downloading this file.
 *
 * This source forms part of the Software and can be used for educational,
 * training, and demonstration purposes but cannot be used for derivative
 * works except in cases where the derivative works require OVP technology
 * to run.
 *
 * For open source models released under licenses that you can use for
 * derivative works, please visit www.OVPworld.org or www.imperas.com
 * for the location of the open source models.
 *
 */

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

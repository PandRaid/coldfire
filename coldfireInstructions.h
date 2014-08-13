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

// Definition of masks for COLDFIRE instructions

#ifndef COLDFIREINSTRUCTIONS_H
#define COLDFIREINSTRUCTIONS_H

#define WIDTH(_W, _ARG)  ((_ARG) & ((1<<(_W))-1))

////////////////////////////////////////////////////////////////////////////////
// OPCODE FORM
// INTEGER INSTRUCTIONS
// OPCODE(4) R1(3) OPMODE(3) ADDRMODE(3) R2(3)
////////////////////////////////////////////////////////////////////////////////
#define OP2_R1(_I)       WIDTH(3,(_I)>>9)
#define OP2_R2(_I)       WIDTH(3,(_I)>>0)
#define OP2_MODE(_I)	 WIDTH(3, (_I)>>6)

////////////////////////////////////////////////////////////////////////////////
// OPCODE FORM
// IMMEDIATE INSTRUCTIONS
// OPCODE(13) R1(3) IMMEDIATE(32)
////////////////////////////////////////////////////////////////////////////////
#define OP3_R1(_I)       WIDTH(3,(_I)>>32)
#define OP3_IMML(_I)	     WIDTH(16, (_I)>>0)
#define OP3_IMMU(_I)	     WIDTH(16, (_I)>>16)

#endif

	
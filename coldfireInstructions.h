/*
This is a header file the gives the layout of different types of
coldfire instructions
*/

// Definition of masks for COLDFIRE instructions

#ifndef COLDFIREINSTRUCTIONS_H
#define COLDFIREINSTRUCTIONS_H

//shifting macro
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

////////////////////////////////////////////////////////////////////////////////
// OPCODE FORM JUMP (16 32 or 48)
// OPCODE(6) N(26)
////////////////////////////////////////////////////////////////////////////////
#define OP7_REGA(_I)       WIDTH(3,(_I)>>0)
#define OP7_EXT16(_I)       WIDTH(16,(_I)>>0)

////////////////////////////////////////////////////////////////////////////////
// OPCODE FORM BRANCH (16 32 or 48)
// OPCODE(6) N(26)
////////////////////////////////////////////////////////////////////////////////
#define OP1_COND(_I, size)       WIDTH(4,(_I)>>(size-16+8))
#define OP1_DISP(_I, size)       WIDTH(8,(_I)>>(size-16))
#define OP1_EXT16(_I)       WIDTH(16,(_I)>>0)
#define OP1_EXT16P2(_I)	     WIDTH(16, (_I)>>16)

#endif

	
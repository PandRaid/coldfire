// Definition of masks for coldfire instructions

#ifndef COLDFIREINSTRUCTIONS_H
#define COLDFIREINSTRUCTIONS_H

#define WIDTH(_W, _ARG)  ((_ARG) & ((1<<(_W))-1))

////////////////////////////////////////////////////////////////////////////////
// OPCODE FORM
// INTEGER INSTRUCTIONS
// OPCODE(4) R1(3) OPMODE(3) ADDRMODE(3) R2(3)
////////////////////////////////////////////////////////////////////////////////
#define OP2_R1(_I)       WIDTH(3,(_I)>>8)
#define OP2_R2(_I)       WIDTH(3,(_I)>>0)
#define OP2_MODE(_I)	 WIDTH(3, (_I)>>5)

////////////////////////////////////////////////////////////////////////////////
// OPCODE FORM
// IMMEDIATE INSTRUCTIONS
// OPCODE(13) R1(3) IMMEDIATE(32)
////////////////////////////////////////////////////////////////////////////////
#define OP3_R1(_I)       WIDTH(3,(_I)>>0)
#define OP3_IMML(_I)	     WIDTH(16, (_I)>>16)
#define OP3_IMMU(_I)	     WIDTH(16, (_I)>>16)

#endif
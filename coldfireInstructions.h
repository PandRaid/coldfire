// Definition of masks for coldfire instructions

#ifndef COLDFIREINSTRUCTIONS_H
#define COLDFIREINSTRUCTIONS_H

#define WIDTH(_W, _ARG)  ((_ARG) & ((1<<(_W))-1))

////////////////////////////////////////////////////////////////////////////////
// OPCODE FORM
// OPCODE(6) D(5) A(5) I(16)
////////////////////////////////////////////////////////////////////////////////
#define OP2_D(_I)       WIDTH(5,(_I)>>21)
#define OP2_A(_I)       WIDTH(5,(_I)>>16)
#define OP2_I(_I)       WIDTH(16,(_I)>>0)

#endif
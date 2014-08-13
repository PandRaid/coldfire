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

// Utility routines and structures for the COLDFIRE decoder

#ifndef COLDFIREDECODE_H
#define COLDFIREDECODE_H

// basic number types
#include "hostapi/impTypes.h"

// model header files
#include "coldfireStructure.h"

//
// Dispatcher callback type
//
#define COLDFIRE_DISPATCH_FN(_NAME) void _NAME( \
    coldfireP coldfire,     \
    Uns32 thisPC,   \
    Uns64 instr,    \
    void *userData  \
)
typedef COLDFIRE_DISPATCH_FN((*coldfireDispatchFn));


//
// Instruction type enumeration
//
typedef enum coldfireInstructionTypeE {

    //INTEGER INSTRUCTIONS 16 bit
    COLDFIRE_ADD,

    //INTEGER INSTRUCTIONS 32 bit
    //INTEGER INSTRUCTIONS 48 bit
    COLDFIRE_ADDI,

    //Fall through for 16,32,48 bit
    COLDFIRE_INSTR16,
    CODLFIRE_INSTR32,
    COLDFIRE_INSTR48,

    // KEEP LAST: for sizing the array
    COLDFIRE_LAST

} coldfireInstructionType;

//
// Dispatch table
//
typedef const coldfireDispatchFn coldfireDispatchTableC[COLDFIRE_LAST];
typedef coldfireDispatchTableC *coldfireDispatchTableCP;

//
// info structure accessed from decoder
//
typedef struct coldfireInstructionInfoS {
    //
    // Default structure members
    //
    const char *opcode;
    Uns32 thisPC;
    Uns32 nextPC;
    Uns64 instruction;
    Uns8  instrsize;
    coldfireInstructionType type;

} coldfireInstructionInfo;

//
// Decode the coldfire instruction at the passed address. If the decode succeeds,
// dispatch it to the corresponding function in the dispatch table and return
// True; otherwise, dispatch using the defaultCB and return False.
//
Bool coldfireDecode(
    coldfireP               coldfire,
    Uns32               thisPC,
    coldfireDispatchTableCP table,
    coldfireDispatchFn      defaultCB,
    void               *userData
);

Uns32 coldfireNextAddr(
    coldfireP               coldfire,
    Uns32               thisPC);

#endif


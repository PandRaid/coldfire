// This is the header file that contains
// Utility routines, instruction enumeration
// info structures for processor

#ifndef COLDFIREDECODE_H
#define COLDFIREDECODE_H

// basic number types
#include "hostapi/impTypes.h"

// model header files
#include "coldfireStructure.h"

//
// Instruction type enumeration
//
typedef enum coldfireInstructionTypeE {

    //INTEGER INSTRUCTIONS 16 bit
    COLDFIRE_ADD,
    COLDFIRE_ADDA,
    COLDFIRE_AND,
    COLDFIRE_SUB,
    COLDFIRE_OR,
    COLDFIRE_DIVU,
    COLDFIRE_EOR,
    COLDFIRE_MULU,
    COLDFIRE_NOT,
    COLDFIRE_SUBA,

    //INTEGER INSTRUCTIONS 32 bit
    //INTEGER INSTRUCTIONS 48 bit
    COLDFIRE_ADDI,
    COLDFIRE_ANDI,
    COLDFIRE_ORI,
    COLDFIRE_SUBI,
    COLDFIRE_EORI,

    //JUMP INSTRUCTIONS
    COLDFIRE_BCC,
    COLDFIRE_J,
    COLDFIRE_JSR,
    COLDFIRE_RTS,

    //Fall through for 16,32,48 bit
    COLDFIRE_INSTR16,
    CODLFIRE_INSTR32,
    COLDFIRE_INSTR48,

    // KEEP LAST: for sizing the array
    COLDFIRE_LAST

} coldfireInstructionType;

//
// Instruction length enumeration
//
typedef enum coldfireLengthE {

    COLDFIRE_16,
    COLDFIRE_32,
    COLDFIRE_48,

} coldfireInstructionLength;

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
    Uns8  instrSize;
    coldfireInstructionType type;

} coldfireInstructionInfo;

typedef coldfireInstructionInfo *coldfireInstructionInfoP;

//
// Dispatcher callback type
//
#define COLDFIRE_DISPATCH_FN(_NAME) void _NAME( \
    coldfireP coldfire,     \
    coldfireInstructionInfoP info,   \
    void *userData  \
)
typedef COLDFIRE_DISPATCH_FN((*coldfireDispatchFn));


//
// Dispatch table
//
typedef const coldfireDispatchFn coldfireDispatchTableC[COLDFIRE_LAST];
typedef coldfireDispatchTableC *coldfireDispatchTableCP;

//
// Decode the coldfire instruction at the passed address. If the decode succeeds,
// dispatch it to the corresponding function in the dispatch table and return
// True; otherwise, dispatch using the defaultCB and return False.
//
Bool coldfireDecode(
    coldfireP               coldfire,
    coldfireDispatchTableCP table,
    coldfireDispatchFn      defaultCB,
    coldfireInstructionInfoP   info,
    void               *userData
);

// Function prototype for next address
Uns32 coldfireNextAddr(
    coldfireP               coldfire,
    coldfireInstructionInfoP   info);

#endif


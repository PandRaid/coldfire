// Utility routines and structures for the coldfire decoder

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
    Uns32 instr,    \
    void *userData  \
)
typedef COLDFIRE_DISPATCH_FN((*coldfireDispatchFn));

//
// Instruction type enumeration
//
typedef enum coldfireInstructionTypeW{

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
// Decode the COLDFIRE instruction at the passed address. If the decode succeeds,
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

#endif

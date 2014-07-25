// standard includes
#include <stdio.h>

// VMI header files
#include "vmi/vmiCxt.h"

// model header files
#include "coldfireDecode.h"
#include "coldfireFunctions.h"


//
// Default disassembler callback
//
static COLDFIRE_DISPATCH_FN(disDefault) {

    // get output buffer for disassembly
    char *result = (char *)userData;

    // default disassembly just shows instruction pattern
    sprintf(result, "??? instruction:0x%08x", instr);
}

//
// OR1K disassembler dispatch table
//
static coldfireDispatchTableC dispatchTable = {

    // handle arithmetic instructions
    [COLDFIRE_ADDI]  = disDefault,
    [COLDFIRE_ADDIC] = disDefault,
    [COLDFIRE_ANDI]  = disDefault,
    [COLDFIRE_ORI]   = disDefault,
    [COLDFIRE_XORI]  = disDefault,
    [COLDFIRE_MULI]  = disDefault
};

//
// COLDFIRE disassembler
//
VMI_DISASSEMBLE_FN(coldfireDisassemble) {
    
    // static buffer to hold disassembly result
    static char result[256];

    // disassemble, filling the result buffer
    coldfireDecode((coldfireP)processor, thisPC, &dispatchTable, disDefault, &result);

    // return the result
    return result;
}


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

// VMI header files
#include "vmi/vmiCxt.h"
#include "vmi/vmiDecode.h"
#include "vmi/vmiMessage.h"
#include <stdint.h>

// model header files
#include "coldfireDecode.h"
#include "coldfireInstructions.h"

//
// This macro adds a decode table entry for a specific instruction class
//
#define DECODE_ENTRY(_PRIORITY, _NAME, _FMT) \
    vmidNewEntryFmtBin(     \
        table,              \
        #_NAME,             \
        COLDFIRE_##_NAME,    \
        _FMT,               \
        _PRIORITY           \
    )
;
//
// Create the coldfire decode table
//
static vmidDecodeTableP createDecodeTable(void) {
    
    vmidDecodeTableP table = vmidNewDecodeTable(16, COLDFIRE_LAST);

    // handle arithmetic instructions (second argument constant)
    DECODE_ENTRY(0, ADD,   "|1101............|"); 
    DECODE_ENTRY(0, ADDI,  "|0000011010000...|");

    return table;
}
//
// Return True if the instruction is a 16-bit instruction
//
inline static Bool is16BitInstruction(Uns16 msw) {
    return ((msw>>11) != 0x00);
}
//
// Return True if the instruction is a 32-bit instruction
//
inline static Bool is32BitInstruction(Uns32 msw) {
    //needs implementation
    return False;
}

//
// Decode an instruction
//
static coldfireInstructionType decode(coldfireP coldfire, Uns16 instr) {

    static vmidDecodeTableP decodeTable;

    // create 48-bit instruction decode table if required
    if(!decodeTable) {
        decodeTable = createDecodeTable();
    }
    coldfireInstructionType type = vmidDecode(decodeTable, instr);

    return type;
}

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
    void               *userData) {

    // get the most-significant two bytes of the instruction
    vmiProcessorP processor = (vmiProcessorP) coldfire;
    Uns64         instr16   = vmicxtFetch2Byte(processor, thisPC);

    // is this a 16-bit or 32-bit instruction?
    if(is16BitInstruction(instr16)) {

        // 16-bit instruction decode
        coldfireInstructionType type = decode(coldfire, instr16);
        if(type!=COLDFIRE_LAST) {
            ((*table)[type])(coldfire, thisPC, instr16, userData);
            return True;
        } 
        else {
            defaultCB(coldfire, thisPC, instr16, userData);
            return False;
        }

    } 
    else if(is32BitInstruction(instr16)){

        // get 32-bit instruction
        Uns64 instr32 = (instr16<<16) | vmicxtFetch2Byte(processor, thisPC+2);
        // 32-bit instruction decode
        coldfireInstructionType type = decode(coldfire, instr16);
        if(type!=COLDFIRE_LAST) {
            ((*table)[type])(coldfire, thisPC, instr32, userData);
            return True;
        } 
        else {
            defaultCB(coldfire, thisPC, instr32, userData);
            return False;
        }
    }
    else{
        // get 48-bit instruction
        uint64_t IMM = vmicxtFetch4Byte(processor, thisPC+2);
        uint64_t shiftedInstr = ((uint64_t) instr16) << 32;
        uint64_t instr48 = IMM | shiftedInstr;
        // 48-bit instruction decode
        coldfireInstructionType type = decode(coldfire, instr16);
        //if(type == COLDFIRE_ADDI)
          //  vmiPrintf("Instr16 %x IMM %llx Instr 64 %12llx\n", (unsigned) instr16, (unsigned long long) IMM, (unsigned long long) instr48);
        if(type!=COLDFIRE_LAST) {
            ((*table)[type])(coldfire, thisPC, instr48, userData);
            return True;
        } 
        else {
            defaultCB(coldfire, thisPC, instr48, userData);
            return False;
        }
    }
    
}

Uns32 coldfireNextAddr(
    coldfireP               coldfire,
    Uns32               thisPC) {

    // get the most-significant two bytes of the instruction
    vmiProcessorP processor = (vmiProcessorP) coldfire;
    Uns16         instr16   = vmicxtFetch2Byte(processor, thisPC);
    Uns32         nextPC=0;

    // is this a 16-bit or 32-bit instruction?
    if(is16BitInstruction(instr16)) {
        nextPC = thisPC + 2;

    } 
    else if(is32BitInstruction(instr16)){
        nextPC = thisPC + 4;
    }
    else{
        nextPC = thisPC + 6;
    }
    return nextPC;
}


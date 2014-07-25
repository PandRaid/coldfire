// VMI header files
#include "vmi/vmiCxt.h"
#include "vmi/vmiDecode.h"

// model header files
#include "coldfireDecode.h"


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

//
// Create the coldfire decode table
//
static vmidDecodeTableP createDecodeTable(void) {
    
    vmidDecodeTableP table = vmidNewDecodeTable(32, COLDFIRE_LAST);

    // handle arithmetic instructions (second argument constant)
    DECODE_ENTRY(0, ADDI,  "|100111..........................|");
    DECODE_ENTRY(0, ADDIC, "|101000..........................|");
    DECODE_ENTRY(0, ANDI,  "|101001..........................|");
    DECODE_ENTRY(0, ORI,   "|101010..........................|");
    DECODE_ENTRY(0, XORI,  "|101011..........................|");
    DECODE_ENTRY(0, MULI,  "|101100..........................|");

    return table;
}

//
// Decode the instruction and return an enum describing it
//
static coldfireInstructionType decode(Uns32 instruction) {

    // get the coldfire decode table
    static vmidDecodeTableP decodeTable;
    if(!decodeTable) {
        decodeTable = createDecodeTable();
    }

    // decode the instruction to get the type
    coldfireInstructionType type = vmidDecode(decodeTable, instruction);

    return type;
}

//
// Decode the coldfire instruction at the passed address. If the decode succeeds,
// dispatch it to the corresponding function in the dispatch table and return
// True; otherwise, dispatch using the defaultCB and return False.
//
Bool or1kDecode(
    coldfireP               coldfire,
    Uns32               thisPC,
    coldfireDispatchTableCP table,
    coldfireDispatchFn      defaultCB,
    void               *userData
) {
    // get the instruction at the passed address - always 4 bytes on coldfire
    vmiProcessorP       processor   = (vmiProcessorP)coldfire;
    Uns32               instruction = vmicxtFetch4Byte(processor, thisPC);
    coldfireInstructionType type        = decode(instruction);

    // apply the callback, or the default if no match
    if(type!=COLDFIRE_LAST) {
        ((*table)[type])(coldfire, thisPC, instruction, userData);
        return True;
    } else {
        defaultCB(coldfire, thisPC, instruction, userData);
        return False;
    }
}


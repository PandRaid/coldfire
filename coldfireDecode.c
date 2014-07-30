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
// Create the coldfire decode table 16 bit
//
static vmidDecodeTableP createDecodeTable16(void) {
    
    vmidDecodeTableP table = vmidNewDecodeTable(16, COLDFIRE_LAST);

    // handle arithmetic instructions (second argument constant)
    DECODE_ENTRY(0, ADD,  "|1101...111......|");

    return table;
}

//
// Create the coldfire decode table 32 bit
//
static vmidDecodeTableP createDecodeTable32(void) {
    
    vmidDecodeTableP table = vmidNewDecodeTable(32, COLDFIRE_LAST);

    // handle arithmetic instructions (second argument constant)

    return table;
}

//
// Create the coldfire decode table 48 bit
//
static vmidDecodeTableP createDecodeTable48(void) {
    
    vmidDecodeTableP table = vmidNewDecodeTable(48, COLDFIRE_LAST);

    // handle arithmetic instructions (second argument constant)
    DECODE_ENTRY(0, ADDI,  "|0000011010000...|");

    return table;
}

//
// Decode a 16-bit instruction
//
static coldfireInstructionType decode16(coldfireP coldfire, Uns16 instr16) {

    static vmidDecodeTableP decodeTable16;

    // create 16-bit instruction decode table if required
    if(!decodeTable16) {
        decodeTable16 = createDecodeTable16();
    }
    coldfireInstructionType type = vmidDecode(decodeTable16, instr16);

    return type;
}

//
// Decode a 32-bit instruction
//
static coldfireInstructionType decode32(coldfireP coldfire, Uns32 instr32) {

    static vmidDecodeTableP decodeTable32;

    // create 32-bit instruction decode table if required
    if(!decodeTable32) {
        decodeTable32 = createDecodeTable32();
    }
    coldfireInstructionType type = vmidDecode(decodeTable32, instr32);

    return type;
}

//
// Decode a 48-bit instruction
//
static coldfireInstructionType decode48(coldfireP coldfire, Uns64 instr48) {

    static vmidDecodeTableP decodeTable48;

    // create 48-bit instruction decode table if required
    if(!decodeTable48) {
        decodeTable48 = createDecodeTable48();
    }
    coldfireInstructionType type = vmidDecode(decodeTable48, instr48);

    return type;
}

//
// Return True if the instruction is a 16-bit instruction
//
inline static Bool is16BitInstruction(Uns16 msw) {
    return ((msw>>11) >= 0x00);
}
//
// Return True if the instruction is a 32-bit instruction
//
inline static Bool is32BitInstruction(Uns32 msw) {
    //needs implementation
    return False;
}

//
// Decode the coldfire instruction at the passed address
Bool coldfireDecode(
    coldfireP               coldfire,
    Uns32               thisPC,
    coldfireDispatchTableCP table,
    coldfireDispatchFn      defaultCB,
    void               *userData) {

    // get the most-significant two bytes of the instruction
    vmiProcessorP processor = (vmiProcessorP) coldfire;
    Uns16         instr16   = vmicxtFetch2Byte(processor, thisPC);


    // is this a 16-bit or 32-bit instruction?
    if(is16BitInstruction(instr16)) {

        // 16-bit instruction decode
        coldfireInstructionType type = decode16(coldfire, instr16);
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
        Uns32 instr32 = (instr16<<16) | vmicxtFetch2Byte(processor, thisPC+2);

        // 32-bit instruction decode
        coldfireInstructionType type = decode32(coldfire, instr32);
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
        Uns64 instr48 = ((Uns64) instr16<<32) | vmicxtFetch2Byte(processor, thisPC+4);

        // 48-bit instruction decode
        coldfireInstructionType type = decode48(coldfire, instr48);
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
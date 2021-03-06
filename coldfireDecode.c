/**
This file is just like a standard decoder. It takes the dissasmbled instructions
and determines what type of instruction it is. It also determines the length of
the instructions and how many bytes to advance
**/

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
// Create the coldfire decode table which maps bit sequences to instructions
//
static vmidDecodeTableP createDecodeTable(void) {
    
    vmidDecodeTableP table = vmidNewDecodeTable(16, COLDFIRE_LAST);

    // handle arithmetic instructions
    DECODE_ENTRY(0, ADD,   "|1101....10......|"); 
    DECODE_ENTRY(0, ADDA,  "|1101...111......|");
    DECODE_ENTRY(0, AND,   "|1100....10......|");
    DECODE_ENTRY(0, SUB,   "|1001....10......|");
    DECODE_ENTRY(0, OR,    "|1000....10......|");
    DECODE_ENTRY(0, MULU,  "|1100...011......|"); 
    DECODE_ENTRY(0, DIVU,  "|1000...011......|");
    DECODE_ENTRY(0, EOR,   "|1011...110......|");
    DECODE_ENTRY(0, NOT,   "|0100011010000...|");
    DECODE_ENTRY(0, SUBA,  "|1001...111......|");
    
    //handle immediate instr
    DECODE_ENTRY(0, ANDI,  "|0000001010000...|");
    DECODE_ENTRY(0, ADDI,  "|0000011010000...|");
    DECODE_ENTRY(0, ORI,   "|0000000010000...|");
    DECODE_ENTRY(0, SUBI,  "|0000010010000...|");
    DECODE_ENTRY(0, EORI,  "|0000101010000...|");

    // handle branch/jump instructions
    DECODE_ENTRY(0, BCC,    "|0110............|");
    DECODE_ENTRY(0, J,      "|0100111011......|");
    DECODE_ENTRY(0, JSR,     "|0100111010......|");
    DECODE_ENTRY(0, RTS,     "|0100111001110101|");

    return table;
}

//This function is for determining the length of the instruction
//since coldfire supports 16, 32 and 48 bit instructions
void static getInstructionLength(coldfireInstructionInfoP info){
    Uns16 msw = info->instruction;
    switch(info->type){
        //variable bit
        case COLDFIRE_ADDA:
        case COLDFIRE_ADD:
        case COLDFIRE_AND:
        case COLDFIRE_SUB:
        case COLDFIRE_SUBA:
        case COLDFIRE_NOT:
        case COLDFIRE_EOR:
        case COLDFIRE_DIVU:
        case COLDFIRE_MULU:
        case COLDFIRE_J:
        case COLDFIRE_JSR:
        case COLDFIRE_RTS:
            if((msw & 0x003F) != 0x003a)
                info->instrSize = 16;
            else if((msw & 0x003F) == 0x003a)
                info->instrSize = 32;
            break;
        case COLDFIRE_BCC:
            if((msw & 0x00FF) != 0x00FF)
                info->instrSize = 32;
            else if((msw & 0x003F) == 0x003a)
                info->instrSize = 48;
            break;
        //Fixed 16 bit
        //Fixed 32 bit
        //Fixed 48 Bit
        case COLDFIRE_ANDI:
        case COLDFIRE_ADDI:
        case COLDFIRE_EORI:
            info->instrSize = 48;
            break;
        default:
            info->instrSize = 48;
    }
}

//
// Decode instruction at designated address
static coldfireInstructionType decode(coldfireP coldfire, Uns16 instr) {

    static vmidDecodeTableP decodeTable;

    // create instruction decode table if required, 16 bits is used
    // since all instructions start with 16 bits and extend if necesarry
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
// Determine the instruction length
Bool coldfireDecode(
    coldfireP               coldfire,
    coldfireDispatchTableCP table,
    coldfireDispatchFn      defaultCB,
    coldfireInstructionInfoP info,
    void               *userData) {

    // get the most-significant two bytes of the instruction
    vmiProcessorP processor = (vmiProcessorP) coldfire;
    Uns32 thisPC = info->thisPC;
    Uns64         instr16   = vmicxtFetch2Byte(processor, thisPC);
    coldfireInstructionType type = decode(coldfire, instr16);
    info->type = type;
    info->instruction = instr16;
    getInstructionLength(info);

    // is this a 16-bit or 32-bit instruction?
    if(info->instrSize == 16) {
        info->instruction = instr16;   //set info structure with size

        // dispatch to the corresponding function in the dispatch table
        if(type!=COLDFIRE_LAST) {
            ((*table)[type])(coldfire, info, userData);
            return True;
        } 
        else {
            defaultCB(coldfire, info, userData);
            return False;
        }

    } 
    else if(info->instrSize == 32){

        // get 32-bit instruction
        Uns64 instr32 = (instr16<<16) | vmicxtFetch2Byte(processor, thisPC+2);
        info->instruction = instr32;   //set info structure with size

        // dispatch to the corresponding function in the dispatch table
        if(type!=COLDFIRE_LAST) {
            ((*table)[type])(coldfire, info, userData);
            return True;
        } 
        else {
            defaultCB(coldfire, info, userData);
            return False;
        }
    }
    else{
        // get 48-bit instruction
        uint64_t IMM = vmicxtFetch4Byte(processor, thisPC+2);
        uint64_t shiftedInstr = ((uint64_t) instr16) << 32;
        uint64_t instr48 = IMM | shiftedInstr;
        info->instruction = instr48;  //set info structure with size

        // dispatch to the corresponding function in the dispatch table
        if(type!=COLDFIRE_LAST) {
            ((*table)[type])(coldfire, info, userData);
            return True;
        } 
        else {
            defaultCB(coldfire, info, userData);
            return False;
        }
    }
    
}

//function for determining the next address to move to
//Used in the utils Next address function
Uns32 coldfireNextAddr(
    coldfireP               coldfire,
    coldfireInstructionInfoP    info) {

    // get the most-significant two bytes of the instruction
    vmiProcessorP processor = (vmiProcessorP) coldfire;
    info->instruction   = vmicxtFetch2Byte(processor, info->thisPC);
    info->type = decode(coldfire, info->instruction);
    getInstructionLength(info);

    //advance instruction based on instruction length
    if(info->instrSize == 16) {
        info->nextPC = info->thisPC + 2;
    } 
    else if(info->instrSize == 32){
        info->nextPC = info->thisPC + 4;
    }
    else if(info->instrSize == 48){
        info->nextPC = info->thisPC + 6;
    }
    else{
        info->nextPC = info->thisPC + 2;
    }
    return info->nextPC;
}


/*
File for getting and setting register information
Contains debug support for register dumping
*/

// VMI header files
#include "vmi/vmiCxt.h"
#include "vmi/vmiMt.h"
#include "vmi/vmiRt.h"
#include "vmi/vmiMessage.h"

// model header files
#include "coldfireDecode.h"
#include "coldfireFunctions.h"
#include "coldfireInstructions.h"
#include "coldfireStructure.h"

//
// Set the value of a bit in a word
//
#define SET_BIT(_R, _C, _M) \
    if(_C) {                    \
        (_R) = (_R) | (_M);     \
    } else {                    \
        (_R) = (_R) & ~(_M);    \
    }

//
// Get the value of a bit in a word
//
#define GET_BIT(_R, _M) \
    (((_R) & (_M)) ? 1 : 0)

//
// Fill the full current value of the SR register, including bits normally
// maintained in separate flags fields
//
inline static void fillSR(coldfireP coldfire) {
    SET_BIT(coldfire->CCR, coldfire->carryFlag,    SPR_SR_CY);
    SET_BIT(coldfire->CCR, coldfire->overflowFlag, SPR_SR_OVE);
}

//
// Return the current value of the SR register
//
Uns8 coldfireGetSR(coldfireP coldfire) {
    fillSR(coldfire);
    return coldfire->CCR;
}

//
// Set the current value of the SR register
//
void coldfireSetSR(coldfireP coldfire, Uns8 value) {

    // first 3 bits on lower 8 bits of SR always cleared
    value &= 0b00011111;

    // set the SR
    coldfire->CCR = value;

    // set the carry flag and overflow flag from the SR
    coldfire->carryFlag    = GET_BIT(value, SPR_SR_CY);
    coldfire->overflowFlag = GET_BIT(value, SPR_SR_OVE);
}

//
// This returns the endianness of the OR1K processor
//
VMI_ENDIAN_FN(coldfireGetEndian) {
    return MEM_ENDIAN_BIG;
}

//
// Return the next instruction address after 'thisPC'.
//
VMI_NEXT_PC_FN(coldfireNextInstruction) {
    coldfireInstructionInfo info = {0};
    info.thisPC = thisPC;
    Uns32 nextAddress = coldfireNextAddr((coldfireP)processor, &info);
    return nextAddress;
}

//
// Dump COLDFIRE registers
//
VMI_DEBUG_FN(coldfireDumpRegisters) {
    
    coldfireP coldfire = (coldfireP)processor;
    Uns32 i    = 0;

    vmiPrintf("--------------- --------------- --------------- ---------------\n");

    // print general-purpose registers
    while(i<COLDFIRE_REGSD) {
        vmiPrintf(" d%-2u: %08x", i, coldfire->regsD[i]);
        i++;
        if(!(i&3)) {
            vmiPrintf("\n");
        } else {
            vmiPrintf("  ");
        }
    }

    i=0;
    // print addr registers
    while(i<COLDFIRE_REGSA) {
        vmiPrintf(" a%-2u: %08x", i, coldfire->regsA[i]);
        i++;
        if(!(i&3)) {
            vmiPrintf("\n");
        } else {
            vmiPrintf("  ");
        }
    }

    // newline if required before derived registers
    if(i&3) {
        vmiPrintf("\n");
    }

    // flags
    vmiPrintf(
        " CF:%u OF:%u ",
        coldfire->carryFlag,
        coldfire->overflowFlag
    );

    // program counter
    vmiPrintf(" PC : %08x  ", (Uns32)vmirtGetPC(processor));

    // status register
    vmiPrintf(" CCR : %01x  ", coldfireGetSR(coldfire));

    vmiPrintf("\n--------------- --------------- --------------- ---------------\n\n");
}

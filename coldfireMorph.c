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
#include "vmi/vmiMessage.h"
#include "vmi/vmiMt.h"
#include "vmi/vmiRt.h"

// model header files
#include "coldfireDecode.h"
#include "coldfireFunctions.h"
#include "coldfireInstructions.h"
#include "coldfireStructure.h"

//
// Update carry & overflow flags
//
const vmiFlags flagsCO = {
    COLDFIRE_CARRY_CONST,           // offset to carry in flag
    {
        COLDFIRE_CARRY_CONST,       // offset to carry out flag
        VMI_NOFLAG_CONST,       // parity flag not used
        VMI_NOFLAG_CONST,       // zero flag not used
        VMI_NOFLAG_CONST,       // sign flag not used
        COLDFIRE_OVERFLOW_CONST     // offset to overflow flag
    }
};

//
// Emit code to implement a binary/unsigned 16 bit literal COLDFIRE instruction
//
static void doBinopULit16(Uns64 instr, vmiBinop op, vmiFlagsCP flags, Uns8 instrLength){

    Uns8 mode = OP2_MODE(instr);
    Uns32 rd;
    Uns32 rs;
    if(mode == 2){
        rd = OP2_R1(instr); 
        rs = OP2_R2(instr);
    }
    else if(mode == 6){
        rd = OP2_R2(instr); 
        rs = OP2_R1(instr); 
    }
    else{
        rd = OP2_R1(instr); 
        rs = OP2_R2(instr);
    }

    vmiReg target = COLDFIRE_REGD(rd);
    vmiReg source = COLDFIRE_REGD(rs);
    if(mode == 7){
        source = COLDFIRE_REGA(rs);
    }


    vmimtBinopRRR(COLDFIRE_BITS, op, source, source, target, flags);


}

//
// Emit code to implement a binary/unsigned 48 bit literal COLDFIRE instruction
//
static void doBinopULit48(Uns64 instr, vmiBinop op, vmiFlagsCP flags, Uns8 instrLength){

    Uns64 rd = OP3_R1(instr); 
    Uns32 IMML = OP3_IMML(instr);
    Uns32 IMMU = OP3_IMMU(instr);
    Uns32 Total = (IMMU << 16) | IMML;

    vmiReg target = COLDFIRE_REGD(rd);
    vmimtBinopRRC(COLDFIRE_BITS, op, target, target, Total, flags);
}

//
// Emit code for a jump instruction
//
static void doJump(
    Uns64 instr,
    Uns32 thisPC,
    Bool  link, 
    Uns8 instrLength
) {
    vmiReg      linkReg     = link ? COLDFIRE_LINKREGA : VMI_NOREG;
    vmiJumpHint hint;

    // select an appropriate jump hint
    if(link) {
        hint = vmi_JH_CALL;
    } else {
        hint = vmi_JH_NONE;
    }

    if(instrLength == 16){
        Uns32 reg = OP7_REGA(instr);
        vmiReg source = COLDFIRE_REGA(reg);
        Uns32 nextAddress = thisPC + 2;
        vmimtUncondJumpReg(nextAddress,source, linkReg, hint);
    }
    else if(instrLength == 32){
        Uns32 offset = OP7_EXT16(instr);
        Uns32 nextAddress = thisPC + 4;
        Uns32 toAddress = thisPC + offset + 2;
        vmimtUncondJump(nextAddress,toAddress, linkReg, hint);
    }
}

//
// Handle arithmetic instructions
//
static COLDFIRE_DISPATCH_FN(morphADD)  {doBinopULit16(info->instruction, vmi_ADD, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphADDA) {doBinopULit16(info->instruction, vmi_ADD, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphAND) {doBinopULit16(info->instruction, vmi_AND, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphOR) {doBinopULit16(info->instruction, vmi_OR, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphSUB) {doBinopULit16(info->instruction, vmi_SUB, &flagsCO, info->instrSize);}

//Handle Immediate Instructions
static COLDFIRE_DISPATCH_FN(morphANDI) {doBinopULit48(info->instruction, vmi_AND, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphADDI) {doBinopULit48(info->instruction, vmi_ADD, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphORI) {doBinopULit48(info->instruction, vmi_OR, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphSUBI) {doBinopULit48(info->instruction, vmi_SUB, &flagsCO, info->instrSize);}
//
// Handle branch instructions
//
static COLDFIRE_DISPATCH_FN(morphJ)     {doJump(info->instruction, info->thisPC, False, info->instrSize);}

//
// COLDFIRE morpher dispatch table
static coldfireDispatchTableC dispatchTable = {

    // handle arithmetic instructions
    [COLDFIRE_ADD] = morphADD,
    [COLDFIRE_ADDA] = morphADDA,
    [COLDFIRE_AND] = morphAND,
    [COLDFIRE_OR]  = morphOR,
    [COLDFIRE_SUB]  = morphSUB,

    //handle immediate instructions
    [COLDFIRE_ADDI]  = morphADDI,
    [COLDFIRE_ANDI]  = morphANDI,
    [COLDFIRE_ORI]  = morphORI,
    [COLDFIRE_SUBI]  = morphSUBI,
    // handle branch instructions
    [COLDFIRE_J]     = morphJ,
};

//
// Called for an undecoded instruction
//
static void undecoded(
    vmiProcessorP processor,
    Uns32         thisPC,
    Uns64         instruction
) {
    // report that undecoded instruction was encountered
    vmiPrintf(
        "CPU '%s' 0x%08x:0x%08llx *** undecoded instruction: exiting ***\n",
        vmirtProcessorName(processor),
        thisPC,
        (unsigned long long) instruction
    );

    // exit the CPU
    vmirtExit(processor);
}

//
// Default morpher callback
//
static COLDFIRE_DISPATCH_FN(morphDefault) {

    // print message warning about the undecoded instruction
    vmimtArgProcessor();
    vmimtArgUns32(info->thisPC);
    vmimtArgUns32(info->instruction);
    vmimtCall((vmiCallFn)undecoded);
}

//
// Create code for the coldfire instruction at the simulated address referenced
// by 'thisPC'.
//
VMI_MORPH_FN(coldfireMorphInstruction) {
    coldfireInstructionInfo info = {0};
    info.thisPC = thisPC;
    coldfireDecode((coldfireP)processor, &dispatchTable, morphDefault, &info, 0);
}

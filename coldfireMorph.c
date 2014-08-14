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
// Emit code to implement a binary/signed 16 bit literal COLDFIRE instruction
//
static void doBinopSLit16(Uns64 instr, vmiBinop op, vmiFlagsCP flags){

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
    vmimtBinopRRR(COLDFIRE_BITS, op, COLDFIRE_REGD(rs), COLDFIRE_REGD(rs), target, flags);


}

//
// Emit code to implement a binary/signed 48 bit literal COLDFIRE instruction
//
static void doBinopSLit48(Uns64 instr, vmiBinop op, vmiFlagsCP flags){

    Uns64 rd = OP3_R1(instr); 
    Uns32 IMML = OP3_IMML(instr);
    Uns32 IMMU = OP3_IMMU(instr);
    Uns32 Total = (IMMU << 16) | IMML;

    vmiReg target = COLDFIRE_REGD(rd);
    vmimtBinopRRC(COLDFIRE_BITS, op, target, target, Total, flags);
}

//
// Handle arithmetic instructions
//
static COLDFIRE_DISPATCH_FN(morphADD)  {doBinopSLit16(instr, vmi_ADD, &flagsCO);}
static COLDFIRE_DISPATCH_FN(morphADDI) {doBinopSLit48(instr, vmi_ADD, &flagsCO);}

//
// COLDFIRE morpher dispatch table
static coldfireDispatchTableC dispatchTable = {

    // handle arithmetic instructions
    [COLDFIRE_ADD]  = morphADD,
    [COLDFIRE_ADDI] = morphADDI
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
    vmimtArgUns32((Uns32)thisPC);
    vmimtArgUns32(instr);
    vmimtCall((vmiCallFn)undecoded);
}

//
// Create code for the coldfire instruction at the simulated address referenced
// by 'thisPC'.
//
VMI_MORPH_FN(coldfireMorphInstruction) {
    coldfireDecode((coldfireP)processor, thisPC, &dispatchTable, morphDefault, 0);
}

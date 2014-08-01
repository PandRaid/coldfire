// VMI header files
#include "vmi/vmiMessage.h"
#include "vmi/vmiMt.h"
#include "vmi/vmiRt.h"

// model header files
#include "coldfireDecode.h"
#include "coldfireFunctions.h"
#include "coldfireStructure.h"
#include "coldfireInstructions.h"


//
// Emit code to implement a binary/signed 16 bit literal COLDFIRE instruction
//
static void doBinopSLit16(Uns32 instr, vmiBinop op){

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
    vmimtBinopRRR(COLDFIRE_BITS, op, target, COLDFIRE_REGD(rs), target, 0);


}

//
// Emit code to implement a binary/signed 48 bit literal COLDFIRE instruction
//
static void doBinopSLit48(Uns32 instr, vmiBinop op){

    Uns32 rd = OP3_R1(instr); 
    Uns32 IMML = OP3_IMML(instr);
    Uns32 IMMU = OP3_IMMU(instr);
    Uns32 Total = IMMU + IMML;

    vmiReg target = COLDFIRE_REGD(rd);
    vmimtBinopRRC(COLDFIRE_BITS, op, target, target, Total, 0);
}

//
// Handle arithmetic instructions
//
static COLDFIRE_DISPATCH_FN(morphADD)  {doBinopSLit16(instr, vmi_ADD);}
static COLDFIRE_DISPATCH_FN(morphADDI) {doBinopSLit48(instr, vmi_ADD);}


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
    Uns32         instruction
) {
    // report that undecoded instruction was encountered
    vmiPrintf(
        "CPU '%s' 0x%08x:0x%08x *** undecoded instruction: exiting ***\n",
        vmirtProcessorName(processor),
        thisPC,
        instruction
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
// Create code for the COLDFIRE instruction at the simulated address referenced
// by 'thisPC'.
//
VMI_MORPH_FN(coldfireMorphInstruction) {
    coldfireDecode((coldfireP)processor, thisPC, &dispatchTable, morphDefault, 0);
}


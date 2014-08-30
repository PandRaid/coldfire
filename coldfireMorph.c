/*
This is the file that determines the behavior of
the decoded instructions. The instruction type and length
are passed through the dispatcher and used in computations
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

//Stack pointer, *Note that this is a sketchy temporary fix. I could not find
// a good way of doing stack type operations and the model seems to call morpher
// way more than it should which causes issues in this implementation
static Uns32 sp=0;
static Uns32 sp2=0;

//
// Emit code to implement a binary/unsigned 16 bit literal COLDFIRE instruction
//
static void doBinopULit16(Uns64 instr, vmiBinop op, vmiFlagsCP flags, Uns8 instrLength){

    Uns8 mode = OP2_MODE(instr);
    Uns32 rd;
    Uns32 rs;

    //determine which register should be used as effective address
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

    //get the source and target registers from macro
    vmiReg target = COLDFIRE_REGD(rd);
    vmiReg source = COLDFIRE_REGD(rs);
    if(mode == 7){
        source = COLDFIRE_REGA(rs);
    }

    //do operation between three registers
    vmimtBinopRRR(COLDFIRE_BITS, op, source, source, target, flags);


}

//
// Emit code to implement a binary/unsigned 16 bit literal COLDFIRE instruction with only one argument
//
static void doUnopULit16(Uns64 instr, vmiBinop op, vmiFlagsCP flags, Uns8 instrLength){
    Uns32 rs = OP2_R1(instr);
    vmiReg source = COLDFIRE_REGD(rs);

    //do unary operation
    vmimtUnopR(COLDFIRE_BITS, op, source, flags);
}

//
// Emit code to implement a binary/unsigned 48 bit literal COLDFIRE instruction
//
static void doBinopULit48(Uns64 instr, vmiBinop op, vmiFlagsCP flags, Uns8 instrLength){
    //get immediate data from 48 bit instruction
    Uns64 rd = OP3_R1(instr); 
    Uns32 IMML = OP3_IMML(instr);
    Uns32 IMMU = OP3_IMMU(instr);
    Uns32 Total = (IMMU << 16) | IMML;

    //get register and do operation between two registers(target) and constant
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
    Uns8 instrLength,
    coldfireInstructionType type
) {
    //optimization stuff and register linking for jumps
    vmiReg      linkReg     = link ? COLDFIRE_LINKREGA : VMI_NOREG;
    vmiJumpHint hint;

    // select an appropriate jump hint
    if(link) {
        hint = vmi_JH_CALL;
    } else {
        hint = vmi_JH_NONE;
    }

    //determine wether or not jumping from label or address registers
    if(instrLength == 16){
        Uns32 reg = OP7_REGA(instr);
        vmiReg source = COLDFIRE_REGA(reg);
        Uns32 nextAddress = thisPC + 2;
        if(type == COLDFIRE_JSR){
            vmimtStoreRCO(COLDFIRE_BITS, sp, linkReg, nextAddress, MEM_ENDIAN_BIG, False);  //store sp in memory
            sp2++;                                                      //decrease stack
            if(sp2%2 == 0)
                sp++;
        }
        vmimtUncondJumpReg(nextAddress,source, linkReg, hint);     //jump to instruction
    }
    else if(instrLength == 32){                             //label jump
        Uns32 offset = OP7_EXT16(instr);
        Uns32 nextAddress = thisPC + 4;
        if(type == COLDFIRE_JSR){
            vmimtStoreRCO(COLDFIRE_BITS, sp, linkReg, nextAddress, MEM_ENDIAN_BIG, False);  //store sp in memory
            sp2++;                                      //decrease stack
            if(sp2%2 == 0)
                sp++;
        }
        Uns32 toAddress = thisPC + offset + 2;
        vmimtUncondJump(nextAddress,toAddress, linkReg, hint); //jump to instruction
    }
}

//
// Emit code for a return instruction
//
static void doReturn(
    Uns64 instr,
    Uns32 thisPC,
    Bool  link, 
    Uns8 instrLength,
    coldfireInstructionType type
) {
    vmiReg source = COLDFIRE_TEMP(0);
    Uns32 nextAddress = thisPC ;
    vmiReg      linkReg     = link ? COLDFIRE_LINKREGA : VMI_NOREG;
    if(type == COLDFIRE_RTS){
        vmimtLoadRRO(32,32, 1, source, linkReg, MEM_ENDIAN_BIG, False, False);
        sp2--;
        if(sp2%2 == 0)
            sp--;
    }

    vmimtUncondJumpReg(nextAddress,source, linkReg, vmi_JH_NONE);
}

//
// Emit code for a branch instruction
//
static void doBranch(
    Uns64 instr,
    Uns32 thisPC,
    Bool  link, 
    Uns8 instrLength,
    coldfireInstructionType type,
    coldfireP coldfire
) {

    Bool jumpIfTrue=True;
    Uns32 toAddress=0;

    //determine size of branch offset and calculate new address
    if(instrLength == 32){
       Uns32 offset = OP1_EXT16(instr);
       toAddress = thisPC + offset + 2;
    }
    else if(instrLength == 48){
       Uns32 IMML = OP1_EXT16(instr);
       Uns32 IMMU = OP1_EXT16P2(instr);
       Uns32 offset = (IMMU << 16) | IMML;
       toAddress = thisPC + offset + 2;
    }

    vmiReg condition = COLDFIRE_CARRY;

    //use this for setting condition codes for the bcc functions, right now only carry supported
    switch(OP1_COND(instr,instrLength)){
        case 7:
            break;
        case 4:
        case 5:
            condition = COLDFIRE_CARRY;
            break;
    }

    //jump based on whether corresponding type flag is set
    vmimtCondJump(
            condition,        // flagReg
            jumpIfTrue,         // jumpIfTrue
            0,                  // linkPC
            toAddress,          // toAddress
            VMI_NOREG,          // linkReg
            vmi_JH_NONE       // hint
        );
}

//
// Handle arithmetic instructions
//
static COLDFIRE_DISPATCH_FN(morphADD)  {doBinopULit16(info->instruction, vmi_ADD, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphADDA) {doBinopULit16(info->instruction, vmi_ADD, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphAND) {doBinopULit16(info->instruction, vmi_AND, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphOR) {doBinopULit16(info->instruction, vmi_OR, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphSUB) {doBinopULit16(info->instruction, vmi_SUB, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphDIVU)  {doBinopULit16(info->instruction, vmi_DIV, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphEOR) {doBinopULit16(info->instruction, vmi_XOR, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphMULU) {doBinopULit16(info->instruction, vmi_MUL, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphNOT) {doUnopULit16(info->instruction, vmi_NOT, 0, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphSUBA) {doBinopULit16(info->instruction, vmi_SUB, &flagsCO, info->instrSize);}

//Handle Immediate Instructions
static COLDFIRE_DISPATCH_FN(morphANDI) {doBinopULit48(info->instruction, vmi_AND, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphADDI) {doBinopULit48(info->instruction, vmi_ADD, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphORI) {doBinopULit48(info->instruction, vmi_OR, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphSUBI) {doBinopULit48(info->instruction, vmi_SUB, &flagsCO, info->instrSize);}
static COLDFIRE_DISPATCH_FN(morphEORI) {doBinopULit48(info->instruction, vmi_XOR, &flagsCO, info->instrSize);}

//
// Handle branch instructions
//
static COLDFIRE_DISPATCH_FN(morphJ)     {doJump(info->instruction, info->thisPC, False, info->instrSize, info->type);}
static COLDFIRE_DISPATCH_FN(morphJSR)     {doJump(info->instruction, info->thisPC, True, info->instrSize, info->type);}
static COLDFIRE_DISPATCH_FN(morphRTS)     {doReturn(info->instruction, info->thisPC, False, info->instrSize, info->type);}
static COLDFIRE_DISPATCH_FN(morphBCC)     {doBranch(info->instruction, info->thisPC, False, info->instrSize, info->type, coldfire);}

//
// COLDFIRE morpher dispatch table
static coldfireDispatchTableC dispatchTable = {

    // handle arithmetic instructions
    [COLDFIRE_ADD] = morphADD,
    [COLDFIRE_ADDA] = morphADDA,
    [COLDFIRE_AND] = morphAND,
    [COLDFIRE_OR]  = morphOR,
    [COLDFIRE_SUB]  = morphSUB,
    [COLDFIRE_DIVU] = morphDIVU,
    [COLDFIRE_EOR] = morphEOR,
    [COLDFIRE_MULU] = morphMULU,
    [COLDFIRE_NOT] = morphNOT,
    [COLDFIRE_SUBA] = morphSUBA,

    //handle immediate instructions
    [COLDFIRE_ADDI]  = morphADDI,
    [COLDFIRE_ANDI]  = morphANDI,
    [COLDFIRE_ORI]  = morphORI,
    [COLDFIRE_SUBI]  = morphSUBI,
    [COLDFIRE_EORI]  = morphEORI,
    // handle branch instructions
    [COLDFIRE_J]     = morphJ,
    [COLDFIRE_JSR]     = morphJSR,
    [COLDFIRE_RTS]    = morphRTS,
    [COLDFIRE_BCC]    = morphBCC,
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

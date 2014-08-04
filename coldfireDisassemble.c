// standard includes
#include <stdio.h>

// VMI header files
#include "vmi/vmiCxt.h"

// model header files
#include "coldfireDecode.h"
#include "coldfireFunctions.h"
#include "coldfireInstructions.h"

// Disassemble a binary/signed 16bit instruction
//
static void doBinopSLit16(char *buffer, Uns32 instr, char *coldfireop) {

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

    sprintf(buffer, "%-8s r%u,r%u", coldfireop, rd, rs);
}
// Disassemble a binary/signed 32bit instruction
//
static void doBinopSLit48(char *buffer, Uns32 instr, char *coldfireop) {

    Uns32 rd = OP3_R1(instr); 
    Uns32 IMML = OP3_IMML(instr);
    Uns32 IMMU = OP3_IMMU(instr);
    Uns32 Total = IMML + IMMU;

    sprintf(buffer, "%-8s r%u,r%u", coldfireop, rd, Total);
}

static COLDFIRE_DISPATCH_FN(disADD)  {doBinopSLit16(userData, instr, "add.l");}
static COLDFIRE_DISPATCH_FN(disADDI) {doBinopSLit48(userData, instr, "addi.l");}

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
// COLDFIRE disassembler dispatch table
//
static coldfireDispatchTableC dispatchTable = {

    // handle arithmetic instructions
    [COLDFIRE_ADD] = disADD,
    [COLDFIRE_ADDI]  = disADDI
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


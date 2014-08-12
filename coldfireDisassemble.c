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

// standard includes
#include <stdio.h>

// VMI header files
#include "vmi/vmiCxt.h"

// model header files
#include "coldfireDecode.h"
#include "coldfireInstructions.h"
#include "coldfireFunctions.h"

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

// Disassemble a binary/signed 48bit instruction
//
static void doBinopSLit48(char *buffer, Uns32 instr, char *coldfireop) {

    Uns32 rd = OP3_R1(instr); 
    Uns32 IMML = OP3_IMML(instr);
    Uns32 IMMU = OP3_IMMU(instr);
    Uns32 Total = IMML + IMMU;

    sprintf(buffer, "%-8s #%u,r%u", coldfireop, Total, rd);
}

//
// Handle arithmetic instructions
//
static COLDFIRE_DISPATCH_FN(disADD)  {doBinopSLit16(userData, instr, "add.l");}
static COLDFIRE_DISPATCH_FN(disADDI) {doBinopSLit48(userData, instr, "addi.l");}

//
// COLDFIRE disassembler dispatch table
//
static coldfireDispatchTableC dispatchTable = {

    // handle arithmetic instructions
    [COLDFIRE_ADD] = disADD,
    [COLDFIRE_ADDI]  = disADDI
};


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
// coldfire disassembler
//
VMI_DISASSEMBLE_FN(coldfireDisassemble) {
    
    // static buffer to hold disassembly result
    static char result[256];

    // disassemble, filling the result buffer
    coldfireDecode((coldfireP)processor, thisPC, &dispatchTable, disDefault, &result);

    // return the result
    return result;
}


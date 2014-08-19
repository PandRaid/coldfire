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
#include "vmi/vmiMessage.h"

// model header files
#include "coldfireDecode.h"
#include "coldfireInstructions.h"
#include "coldfireFunctions.h"

// Disassemble a binary/unsigned 16bit instruction
//
static void doBinopULit16(char *buffer, Uns64 instr, char *coldfireop, Uns8 instrLength) {

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

    //if using address registers print with that format
    if(mode == 7){
        sprintf(buffer, "%-8s d%u,a%u", coldfireop, rd, rs);
    }
    else{
        sprintf(buffer, "%-8s d%u,d%u", coldfireop, rd, rs);
    }
}

//
// Emit code to implement a binary/unsigned 16 bit literal COLDFIRE instruction with only one argument
//
static void doUnopULit16(char *buffer, Uns64 instr, char *coldfireop, Uns8 instrLength){
    Uns32 rs = OP2_R1(instr);
    
    sprintf(buffer, "%-8s d%u", coldfireop, rs);


}

// Disassemble a binary/unsigned 48bit instruction
//
static void doBinopULit48(char *buffer, Uns64 instr, char *coldfireop, Uns8 instrLength) {

    Uns64 rd = OP3_R1(instr); 
    Uns32 IMML = OP3_IMML(instr);
    Uns32 IMMU = OP3_IMMU(instr);
    Uns32 Total = (IMMU << 16) | IMML;

    sprintf(buffer, "%-8s #%u,d%u", coldfireop, Total, (unsigned) rd);
}

//
// Disassemble a branch or jump instruction.
//
static void doBranchJump(char *buffer, Uns32 thisPC, Uns64 instr, char *coldfireop, Uns8 instrLength){
    if(instrLength == 16){
        Uns32 reg = OP7_REGA(instr);
        sprintf(buffer, "%-8s  a%u", coldfireop, reg);
    }
    else if(instrLength == 32){
        Uns32 offset = OP7_EXT16(instr);
        Uns32 toAddress = thisPC + offset;
        sprintf(buffer, "%-8s  %x", coldfireop, toAddress);
    }
}

//
// Handle arithmetic instructions
//
static COLDFIRE_DISPATCH_FN(disADD)  {doBinopULit16(userData, info->instruction, "add.l", info->instrSize);}
static COLDFIRE_DISPATCH_FN(disADDA) {doBinopULit16(userData, info->instruction, "adda.l", info->instrSize);}
static COLDFIRE_DISPATCH_FN(disAND)  {doBinopULit16(userData, info->instruction, "and.l", info->instrSize);}
static COLDFIRE_DISPATCH_FN(disOR)   {doBinopULit16(userData, info->instruction, "or.l", info->instrSize);}
static COLDFIRE_DISPATCH_FN(disSUB)  {doBinopULit16(userData, info->instruction, "sub.l", info->instrSize);}
static COLDFIRE_DISPATCH_FN(disMULU)  {doBinopULit16(userData, info->instruction, "mulu.l", info->instrSize);}
static COLDFIRE_DISPATCH_FN(disDIVU) {doBinopULit16(userData, info->instruction, "divu.l", info->instrSize);}
static COLDFIRE_DISPATCH_FN(disNOT)  {doUnopULit16(userData, info->instruction, "not.l", info->instrSize);}
static COLDFIRE_DISPATCH_FN(disEOR)   {doBinopULit16(userData, info->instruction, "eor.l", info->instrSize);}
static COLDFIRE_DISPATCH_FN(disSUBA)  {doBinopULit16(userData, info->instruction, "suba.l", info->instrSize);}


static COLDFIRE_DISPATCH_FN(disANDI) {doBinopULit48(userData, info->instruction, "andi.l", info->instrSize);}
static COLDFIRE_DISPATCH_FN(disADDI) {doBinopULit48(userData, info->instruction, "addi.l", info->instrSize);}
static COLDFIRE_DISPATCH_FN(disORI) {doBinopULit48(userData, info->instruction, "ori.l", info->instrSize);}
static COLDFIRE_DISPATCH_FN(disSUBI) {doBinopULit48(userData, info->instruction, "subi.l", info->instrSize);}
static COLDFIRE_DISPATCH_FN(disEORI) {doBinopULit48(userData, info->instruction, "eori.l", info->instrSize);}

//
// Handle branch instructions
//
static COLDFIRE_DISPATCH_FN(disJ)     {doBranchJump(userData, info->thisPC, info->instruction, "jmp", info->instrSize);}

//
// COLDFIRE disassembler dispatch table
//
static coldfireDispatchTableC dispatchTable = {

    // handle arithmetic instructions
    [COLDFIRE_ADD] = disADD,
    [COLDFIRE_ADDA] = disADDA,
    [COLDFIRE_AND] = disAND,
    [COLDFIRE_OR]  = disOR,
    [COLDFIRE_SUB]  = disSUB,
    [COLDFIRE_DIVU] = disDIVU,
    [COLDFIRE_MULU] = disMULU,
    [COLDFIRE_EOR] = disEOR,
    [COLDFIRE_NOT]  = disNOT,
    [COLDFIRE_SUBA]  = disSUBA,

    //handle immediate instructions
    [COLDFIRE_ADDI]  = disADDI,
    [COLDFIRE_ANDI]  = disANDI,
    [COLDFIRE_ORI]  = disORI,
    [COLDFIRE_SUBI]  = disSUBI,
    [COLDFIRE_EORI]  = disEORI,
    // handle branch instructions
    [COLDFIRE_J]     = disJ,
};


//
// Default disassembler callback
//
static COLDFIRE_DISPATCH_FN(disDefault) {

    // get output buffer for disassembly
    char *result = (char *)userData;

    // default disassembly just shows instruction pattern
    sprintf(result, "??? instruction:0x%08llx", (unsigned long long) info->instruction);
}

//
// coldfire disassembler
//
VMI_DISASSEMBLE_FN(coldfireDisassemble) {
    
    // static buffer to hold disassembly result
    static char result[256];
    coldfireInstructionInfo info = {0};
    info.thisPC = thisPC;

    // disassemble, filling the result buffer
    coldfireDecode((coldfireP)processor, &dispatchTable, disDefault, &info, &result);

    // return the result
    return result;
}


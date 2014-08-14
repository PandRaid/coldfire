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

#ifndef COLDFIRE_STRUCTURE_H
#define COLDFIRE_STRUCTURE_H

// VMI header files
#include "vmi/vmiTypes.h"

#define COLDFIRE_REGSA 8
#define COLDFIRE_REGSD 8
#define COLDFIRE_BITS 32
#define COLDFIRE_LINK  7   //link register to a7 (stack)
#define COLDFIRE_LINKREGA  COLDFIRE_REGA(COLDFIRE_LINK)

// processor structure
typedef struct coldfireS {
	Bool       carryFlag;       // carry flag
    Bool       overflowFlag;    // overflow flag
    Uns8	   CCR;				//Status Register
    Bool	   zeroFlag;		//zero flag
    Bool	   signFlag;			//sign flag
    Bool 	   tempFlag;		//for derived flags
    Uns32 regsD[COLDFIRE_REGSD]; // basic DATA registers
	Uns32 regsA[COLDFIRE_REGSA]; // basic ADDRESS registers
    vmiBusPortP busPorts;       // bus port descriptions
} coldfire, *coldfireP;

// macros to specify target registers in VARIABLE expressions
#define COLDFIRE_OFFSET(_F) 		VMI_CPU_REG(coldfireP, _F)
#define COLDFIRE_REGA(_R) 			COLDFIRE_OFFSET(regsA[_R])
#define COLDFIRE_REGD(_R) 			COLDFIRE_OFFSET(regsD[_R])
#define COLDFIRE_CARRY              COLDFIRE_OFFSET(carryFlag)
#define COLDFIRE_OVERFLOW           COLDFIRE_OFFSET(overflowFlag)
#define COLDFIRE_ZERO				COLDFIRE_OFFSET(zeroFlag)
#define COLDFIRE_SIGN				COLDFIRE_OFFSET(signFlag)
#define COLDFIRE_TEMP				COLDFIRE_OFFSET(tempFlag)

// macros to specify target registers in CONSTANT expressions
#define COLDFIRE_OFFSET_CONST(_F) 		VMI_CPU_REG_CONST(coldfireP, _F)
#define COLDFIRE_REGA_CONST(_R) 		COLDFIRE_OFFSET_CONST(regsA[_R])
#define COLDFIRE_REGD_CONST(_R) 		COLDFIRE_OFFSET_CONST(regsD[_R])
#define COLDFIRE_CARRY_CONST            COLDFIRE_OFFSET_CONST(carryFlag)
#define COLDFIRE_OVERFLOW_CONST         COLDFIRE_OFFSET_CONST(overflowFlag)

// Bit definitions for the SR register
#define SPR_SR_CY       0b00000001  // Carry flag
#define SPR_SR_OVE       0b00000010  // Overflow flag
#define SPR_SR_ZER       0b00000100  // Zero flag
#define SPR_SR_NEG       0b00001000  // Negative flag
#define SPR_SR_EXT       0b00010000  // Extend flag

#endif


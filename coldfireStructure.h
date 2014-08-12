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

// processor structure
typedef struct coldfireS {
    Uns32 regsD[COLDFIRE_REGSD]; // basic DATA registers
	Uns32 regsA[COLDFIRE_REGSA]; // basic ADDRESS registers
    vmiBusPortP busPorts;       // bus port descriptions
} coldfire, *coldfireP;

// macros to specify target registers in VARIABLE expressions
#define COLDFIRE_OFFSET(_F) VMI_CPU_REG(coldfireP, _F)
#define COLDFIRE_REGA(_R) COLDFIRE_OFFSET(regsA[_R])
#define COLDFIRE_REGD(_R) COLDFIRE_OFFSET(regsD[_R])

#endif


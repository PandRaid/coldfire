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
#include "vmi/vmiCxt.h"

// model header files
#include "coldfireFunctions.h"

//
// This returns the endianness of the OR1K processor
//
VMI_ENDIAN_FN(coldfireGetEndian) {
    return MEM_ENDIAN_BIG;
}

//
//The ColdFire architecture instruction word length is limited to 3 sizes: 16, 32, or 48 bits
//
VMI_NEXT_PC_FN(coldfireNextInstruction) {
    Uns32 nextAddress = (Uns32)(thisPC + 8);
    return nextAddress;
}



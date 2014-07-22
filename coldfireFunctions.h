
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

#ifndef COLDFIRE_FUNCTIONS_H
#define COLDFIRE_FUNCTIONS_H

// VMI header files
#include "vmi/vmiAttrs.h"

// constructor & destructor
VMI_CONSTRUCTOR_FN(coldfireConstructor);
VMI_DESTRUCTOR_FN(coldfireDestructor);

// morph function
VMI_MORPH_FN(coldfireMorphInstruction);

// simulation support functions
VMI_ENDIAN_FN(coldfireGetEndian);
VMI_NEXT_PC_FN_VAR(coldfireNextInstruction);

// port functions
VMI_BUS_PORT_SPECS_FN(coldfireGetBusPortSpec);

// Processor Info routine
VMI_PROC_INFO_FN(coldfireProcInfo);

#endif


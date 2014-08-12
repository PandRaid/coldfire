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
VMI_NEXT_PC_FN(coldfireNextInstruction);
VMI_DISASSEMBLE_FN(coldfireDisassemble);

// port functions
VMI_BUS_PORT_SPECS_FN(coldfireGetBusPortSpec);

// Processor Info routine
VMI_PROC_INFO_FN(coldfireProcInfo);

#endif
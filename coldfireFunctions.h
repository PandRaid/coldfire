//Function prototype definitions, based on
//macros from imperas libraries

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

// register access
VMI_DEBUG_FN(coldfireDumpRegisters);

// port functions
VMI_BUS_PORT_SPECS_FN(coldfireGetBusPortSpec);

// Processor Info routine
VMI_PROC_INFO_FN(coldfireProcInfo);

// Imperas intercepted function support
//VMI_INT_RETURN_FN(coldfireIntReturn);
//VMI_INT_RESULT_FN(coldfireIntResult);
//VMI_INT_PAR_FN(coldfireIntPar);

#endif
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


// Imperas header files
#include "hostapi/impAlloc.h"

// VMI header files
#include "vmi/vmiAttrs.h"
#include "vmi/vmiMessage.h"

// model header files
#include "coldfireStructure.h"
#include "coldfireFunctions.h"


//
// Return number of members of an array
//
#define NUM_MEMBERS(_A) (sizeof(_A)/sizeof((_A)[0]))


////////////////////////////////////////////////////////////////////////////////
// BUS PORTS
////////////////////////////////////////////////////////////////////////////////

//
// Create MCF flexbus with 8 bit min and 16 bit max port size.
//
const static vmiBusPort busPorts[] = {
    {"INSTRUCTION", vmi_BP_MASTER, vmi_DOM_CODE, {8,16}, 1},
    {"DATA"       , vmi_BP_MASTER, vmi_DOM_DATA, {8,16}, 0},
};

//
// Allocate bus port specifications
//
static void newBusPorts(coldfireP coldfire) {

    Uns32 i;

    coldfire->busPorts = STYPE_CALLOC_N(vmiBusPort, NUM_MEMBERS(busPorts));

    for(i=0; i<NUM_MEMBERS(busPorts); i++) {
        coldfire->busPorts[i] = busPorts[i];
    }
}

//
// Free bus port specifications
//
static void freeBusPorts(coldfireP coldfire) {

    if(coldfire->busPorts) {
        STYPE_FREE(coldfire->busPorts);
        coldfire->busPorts = 0;
    }
}

//
// Get the next bus port
//
VMI_BUS_PORT_SPECS_FN(coldfireGetBusPortSpec) {

    coldfireP coldfire = (coldfireP)processor;

    if(!prev) {

        // first port
        return coldfire->busPorts;

    } else {

        // port other than the first
        Uns32 prevIndex = (prev-coldfire->busPorts);
        Uns32 thisIndex = prevIndex+1;

        return (thisIndex<NUM_MEMBERS(busPorts)) ? &coldfire->busPorts[thisIndex] : 0;
    }
}

//
// OR1K processor constructor
//
VMI_CONSTRUCTOR_FN(coldfireConstructor) {

    coldfireP coldfire = (coldfireP)processor;

    vmiPrintf("%s called\n", FUNC_NAME);

    // create bus port specifications
    newBusPorts(coldfire);
}

//
// OR1K processor destructor
//
VMI_DESTRUCTOR_FN(coldfireDestructor) {

    coldfireP coldfire = (coldfireP)processor;

    vmiPrintf("%s called\n", FUNC_NAME);

    // free bus port specifications
    freeBusPorts(coldfire);
}


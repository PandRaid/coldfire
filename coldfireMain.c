/*
This file does all the initialization
The main is contained in the constructor which uses the
attrs to create processor and intialize registers
All initilization for hardware based structures are done here
*/


// Imperas header files
#include "hostapi/impAlloc.h"

// VMI header files
#include "vmi/vmiAttrs.h"

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
// Template bus port list
// Coldfire uses flexbus that can do 8 bits or 16 bits
const static vmiBusPort busPorts[] = {
    {"INSTRUCTION", vmi_BP_MASTER, vmi_DOM_CODE, {8,16}, 1},    
    {"DATA"       , vmi_BP_MASTER, vmi_DOM_DATA, {8,16}, 0},
};

//
// Allocate bus port specifications
//
static void newBusPorts(coldfireP coldfire) {

    Uns32 i;

    //allocate and initialize bus ports
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
// coldfire processor constructor
//
VMI_CONSTRUCTOR_FN(coldfireConstructor) {

    coldfireP coldfire = (coldfireP)processor;
    Uns32 i;

    //initialize address, data and status registers
    for(i=0; i<(COLDFIRE_REGSD); i++) {
        coldfire->regsA[i] = 0x00000001;
    }
    for(i=0; i<(COLDFIRE_REGSA); i++) {
        coldfire->regsD[i] = 0x00000001;
    }

    //initialize SR
    coldfire->CCR = 0b00000000;

    // create bus port specifications
    newBusPorts(coldfire);
}

//
// coldfire processor destructor
//
VMI_DESTRUCTOR_FN(coldfireDestructor) {

    coldfireP coldfire = (coldfireP)processor;

    // free bus port specifications
    freeBusPorts(coldfire);
}


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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "icm/icmCpuManager.h"

//
// Simulation main loop
//
static Bool simulate(icmProcessorP processor, Uns64 instructions) {

    icmStopReason stopReason = icmSimulate(processor, instructions);

    switch(stopReason) {

       case ICM_SR_SCHED:
           // hit the scheduler limit
           return True;

       case ICM_SR_EXIT:
           // processor has exited
           return False;

       case ICM_SR_FINISH:
           // simulation must end
           return False;

       default:
           icmPrintf("unimplemented stopReason %u\n", stopReason);
           return False;
    }
}

//
// Main simulation routine
//
int main(int argc, char ** argv) {

    // check for the application program name argument
    if(argc!=2) {
        icmPrintf("%s: expected application name argument\n", argv[0]);
        return -1;
    }

    // initialize CpuManager
    icmInit(0, NULL, 0);

    // create a processor
    icmProcessorP processor = icmNewProcessor(
        "cpu1",             // CPU name
        TYPE_NAME,          // CPU type
        0,                  // CPU cpuId
        0,                  // CPU model flags
        32,                 // address bits
        MORPHER_FILE,       // model file
        MORPHER_SYMBOL,     // morpher attributes
        ICM_ATTR_TRACE,     // enable tracing
        0,                  // user-defined attributes
        0,                  // semi-hosting file
        0                   // semi-hosting attributes
    );

    // load the processor object file
    icmLoadProcessorMemory(processor, argv[1], ICM_LOAD_DEFAULT, False, True);

    // run processor, one instruction at a time
    while(simulate(processor, 1)) {
        // keep going while processor is still running
    }

    icmTerminate();

    return 0;
}

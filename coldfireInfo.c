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
#include "vmi/vmiAttrs.h"
#include "vmi/vmiModelInfo.h"

#include "coldfireFunctions.h"

VMI_PROC_INFO_FN(coldfireProcInfo) {
    static const vmiProcessorInfo info = {
        .vlnv.vendor      = "ovpworld.org"    ,
        .vlnv.library     = "processor"       ,
        .vlnv.name        = "coldfire"            ,
        .vlnv.version     = "1.0"             ,

        .semihost.vendor  = "ovpworld.org"    ,
        .semihost.library = "semihosting"     ,
        .semihost.name    = "coldfireNewlib"      ,
        .semihost.version = "1.0"             ,

        .elfCode          = 33906,
        .endianFixed      = True,
        .endian           = MEM_ENDIAN_BIG,
        .gdbPath          = "$IMPERAS_HOME/lib/$IMPERAS_ARCH/CrossCompiler/or32-elf/bin/or32-elf-gdb" VMI_EXE_SUFFIX,
        .family           = "openCores",
    };
    return &info;
}

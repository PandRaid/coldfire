/*
File that contains the necesarry information for creating the actual
platform model and corresponding proccesor
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
        .gdbPath          = "$IMPERAS_HOME/lib/$IMPERAS_ARCH/CrossCompiler/m68k-elf/bin/m68k-elf-gdb" VMI_EXE_SUFFIX,
        .family           = "openCores",
    };
    return &info;
}

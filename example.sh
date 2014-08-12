#!/bin/sh

# Check Installation supports this example
if [ -e ../../../../bin/${IMPERAS_ARCH}/checkinstall.exe ]; then
  ../../../../bin/${IMPERAS_ARCH}/checkinstall.exe -p install.pkg --nobanner || exit
fi


make all

platform.exe application.elf

platform.exe asmtest.elf

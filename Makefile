IMPERAS_HOME := $(shell getpath.exe "$(IMPERAS_HOME)")
include $(IMPERAS_HOME)/bin/Makefile.include

ifndef IMPERAS_HOME
  IMPERAS_ERROR := $(error "IMPERAS_HOME not defined")
endif

CFLAGS = $(SIM_CFLAGS) $(OTHER_CFLAGS)
LDFLAGS = $(OTHER_LDFLAGS)

ifeq ($(shell uname),Linux)
  LDFLAGS += -Wl,--version-script=version.script
else
  LDFLAGS += export.def
endif

all: or1kModel.$(IMPERAS_SHRSUF) application.elf platform.exe

#
# Processor Section
#
SRCS.c = $(sort $(wildcard *.c))
SRCS.S = $(sort $(wildcard *.S))
OBJS.c = $(foreach obj, $(SRCS.c:.c=.o), $(obj))
OBJS.S = $(foreach obj, $(SRCS.S:.S=.o), $(obj))
OBJS = $(OBJS.c) $(OBJS.S)
SOLIB = or1kModel.$(IMPERAS_SHRSUF)

%.o: %.c
	$(V) echo "Compiling Processor $@"
	$(V) $(HOST_GCC) $(CFLAGS) -c -o $@ $^

$(SOLIB): $(OBJS)
	$(V) echo "Linking Processor"
	$(V) $(HOST_GCC) $(CFLAGS) --shared -o $@ $^ $(IMPERAS_VMISTUBS) $(LDFLAGS)

#
# platform Section
#
MORPHER_FILE   = "./or1kModel.$(IMPERAS_SHRSUF)"
MORPHER_SYMBOL = "modelAttrs"
TYPE_NAME      = "or1k"

platform.exe: platform/platform.o
	$(V) echo "Linking platform $@"
	$(V) $(HOST_GCC) $(CFLAGS) -o $@ $^ $(SIM_LDFLAGS) 

platform/platform.o: platform/platform.c
	$(V) echo "Compiling platform $@"
	$(V) $(HOST_GCC) $(CFLAGS) -c -o $@ $< \
	  -DMORPHER_FILE="\"${MORPHER_FILE}\"" \
	  -DMORPHER_SYMBOL="\"${MORPHER_SYMBOL}\"" \
	  -DTYPE_NAME="\"${TYPE_NAME}\"" 

#
# Application section
#
-include $(IMPERAS_LIB)/CrossCompiler/OR1K.makefile.include
ifeq ($(OR1K_CC),)
    IMPERAS_ERROR := $(error "Please install the OR1K_CC toolchain")
endif

application.elf: application/application.o
	$(V) echo "Linking Application $@"
	$(V) $(IMPERAS_LINK) -o $@  $^  $(IMPERAS_LDFLAGS)

application/application.o: application/application.c
	$(V) echo "Compiling Application $@"
	$(V) $(IMPERAS_CC)  -c  -o $@  $<

#
# Cleanup section
#
clean:
	$(V) - rm -f application.elf application/application.o
	$(V) - rm -f $(OBJS) $(SOLIB)
	$(V) - rm -f platform/platform.o platform.exe

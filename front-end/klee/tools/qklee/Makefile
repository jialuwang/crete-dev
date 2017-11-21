#===-- tools/klee/Makefile ---------------------------------*- Makefile -*--===#
#
#                     The KLEE Symbolic Virtual Machine
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
#===------------------------------------------------------------------------===#

LEVEL=../..
TOOLNAME = qklee

include $(LEVEL)/Makefile.config

USEDLIBS = kleeCore.a kleeBasic.a kleeModule.a  kleaverSolver.a kleaverExpr.a kleeSupport.a 
LINK_COMPONENTS = jit bitreader bitwriter ipo linker engine

ifeq ($(shell python -c "print($(LLVM_VERSION_MAJOR).$(LLVM_VERSION_MINOR) >= 3.3)"), True)
LINK_COMPONENTS += irreader
endif
include $(LEVEL)/Makefile.common

# add -lqfm flag when qfm folder presents
#LIBS += -lstp -lqfm
LIBS += -lstp

QEMU_SRC := ../../../qemu-2.1.2/
CXX.Flags += -I$(QEMU_SRC) -I$(QEMU_SRC)/hw -I$(QEMU_SRC)/fpu \
                 -I$(QEMU_SRC)/include `pkg-config --cflags glib-2.0` \
                 -I$(QEMU_SRC)/include/qemu/ \
                 -I$(QEMU_SRC)/include/net/ -I$(QEMU_SRC)/include/ \
                 -I$(QEMU_SRC)/include/exec/ -I$(QEMU_SRC)/hw/pci/ \
                 -I$(QEMU_SRC)/include/hw/ -I$(QEMU_SRC)/hw/net/ \
                 -I$(QEMU_SRC)/include/hw/pci/ -I/usr/include/x86_64-linux-gnu \
                 -I/usr/include/p11-kit-1    -I/usr/include/libpng12 \
                 -I/usr/include/pixman-1

ifeq ($(ENABLE_METASMT),1)
  include $(METASMT_ROOT)/share/metaSMT/metaSMT.makefile
  LD.Flags += -L$(METASMT_ROOT)/../../deps/Z3-4.1/lib \
              -L$(METASMT_ROOT)/../../deps/boolector-1.5.118/lib \
              -L$(METASMT_ROOT)/../../deps/minisat-git/lib/ \
              -L$(METASMT_ROOT)/../../deps/boost-1_52_0/lib 
  CXX.Flags += -DBOOST_HAS_GCC_TR1
  CXX.Flags := $(filter-out -fno-exceptions,$(CXX.Flags)) 
  LIBS += -lgomp -lboost_iostreams -lboost_thread -lboost_system -lmetaSMT -lz3 -lrt -lboolector -lminisat_core
endif

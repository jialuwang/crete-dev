CC = clang-3.4
CFLAGS = -emit-llvm -S -fno-vectorize -g -Wall
LD = llvm-link-3.4
LDFLAGS =
LIBS =

QEMU_SRC := ../qemu-2.3
QEMU_INCLUDES += -I$(QEMU_SRC) -I$(QEMU_SRC)/hw -I$(QEMU_SRC)/fpu \
                 -I$(QEMU_SRC)/include `pkg-config --cflags glib-2.0` \
		 -I$(QEMU_SRC)/qdh -I$(QEMU_SRC)/include/qemu/ \
		 -I$(QEMU_SRC)/include/net/ -I$(QEMU_SRC)/include/ \
		 -I$(QEMU_SRC)/include/exec/ -I$(QEMU_SRC)/hw/pci/ \
		 -I$(QEMU_SRC)/include/hw/ -I$(QEMU_SRC)/hw/net/ \
		 -I$(QEMU_SRC)/include/hw/pci/ -I/usr/include/x86_64-linux-gnu \
		 -I/usr/include/p11-kit-1    -I/usr/include/libpng12 \
		 -I/usr/include/pixman-1 
#FW_INCLUDES = -I../../emu8051/replay

MAIN_FILE := test.c
REPLAY_VD_FILE := replay_vd.c
FUNC_DEF_FILE := exfunc_def.c


#SOURCES := $(QEMU_SRC)/net/checksum.c
# $(QEMU_SRC)/util/notify.c \
           $(QEMU_SRC)/qemu-timer.c $(QEMU_SRC)/stubs/clock-warp.c \
           $(QEMU_SRC)/util/oslib-posix.c $(QEMU_SRC)/util/compatfd.c \
           $(QEMU_SRC)/net/net.c $(QEMU_SRC)/qapi-types.c 


OBJECTS := ${SOURCES:.c=.bc}
TARGET = test.ll
REPLAY_VD = replay_vd.ll
FUNC_DEF = exfunc_def.ll

.PHONY: all clean distclean

all:
	$(CC) $(CFLAGS) $(FW_INCLUDES) $(QEMU_INCLUDES) $(MAIN_FILE) -o $(TARGET)
	$(CC) $(CFLAGS) $(FW_INCLUDES) $(QEMU_INCLUDES) $(FUNC_DEF_FILE) -o $(FUNC_DEF)
	$(CC) $(CFLAGS) $(FW_INCLUDES) $(QEMU_INCLUDES) $(REPLAY_VD_FILE) -o $(REPLAY_VD)

#l: $(TARGET)
#(TARGET): $(OBJECTS)
#$(CC) $(CFLAGS) $(QEMU_INCLUDES) $(MAIN_FILE) -o $(MAIN_FILE).bc
#$(LD) -o $@ $^ $(MAIN_FILE).bc $(LDFLAGS) $(LIBS)

#.bc : %.c
#$(CC) $(CFLAGS) $(QEMU_INCLUDES) $< -o $@

clean:
#$(RM) $(OBJECTS) $(TARGET) $(TARGET).bc $(MAIN_FILE).bc
	$(RM) $(TARGET)

distclean: clean

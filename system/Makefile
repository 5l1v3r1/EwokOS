# tools
AR = arm-none-eabi-ar
CC = arm-none-eabi-gcc
CXX = arm-none-eabi-c++
LD = arm-none-eabi-ld
OBJDUMP = arm-none-eabi-objdump

export QEMU_AUDIO_DRV=none

ifneq ($(MACH),)
ARCH=arm/$(MACH)
else
ARCH=arm/versatilepb
endif

BUILD_DIR = build
TARGET_DIR = $(BUILD_DIR)/rootfs
ROOT_DIR = .

# flags
CFLAGS = -Wstrict-prototypes \
				 -marm \
				 -pedantic -Wall -Wextra -msoft-float -fPIC -mapcs-frame \
         -fno-builtin-printf \
         -fno-builtin-memset \
         -fno-builtin-calloc \
         -fno-builtin-snprintf \
         -fno-builtin-sprintf \
         -fno-builtin-fprintf \
				 -Wno-overlength-strings \
         -fno-builtin-exit -fno-builtin-stdio \
				 -std=c99 

#CFLAGS +=  -g
CFLAGS +=  -O2

KERNEL_DIR=../kernel
ARCH_DIR=$(KERNEL_DIR)/arch

LDFLAGS = -L$(TARGET_DIR)/lib -lnosys

LIBS_DIR = libs

LIB_INC = -I $(LIBS_DIR)/sys/include \
				  -I $(LIBS_DIR)/sconf/include \
				  -I $(LIBS_DIR)/libc/include \
				  -I $(LIBS_DIR)/x/include \
				  -I $(LIBS_DIR)/ext2/include \
				  -I $(LIBS_DIR)/graph/include \
				  -I $(LIBS_DIR)/libm/include \
				  -I $(LIBS_DIR)/console/include \
					-I $(LIBS_DIR)/mario/include \
					-I $(LIBS_DIR)/upng/include \
					-I $(LIBS_DIR)/tga/include \
					-I $(LIBS_DIR)/hash/include \
					-I $(KERNEL_DIR)/include
				 
LIB_SYS_DIR = $(LIBS_DIR)/sys
LIB_SCONF_DIR = $(LIBS_DIR)/sconf
LIB_LIBC_DIR = $(LIBS_DIR)/libc
LIB_X_DIR = $(LIBS_DIR)/x
LIB_EXT2_DIR = $(LIBS_DIR)/ext2
LIB_GRAPH_DIR = $(LIBS_DIR)/graph
LIB_M_DIR = $(LIBS_DIR)/libm
LIB_CONSOLE_DIR = $(LIBS_DIR)/console
LIB_UPNG_DIR = $(LIBS_DIR)/upng
LIB_TGA_DIR = $(LIBS_DIR)/tga
LIB_HASH_DIR = $(LIBS_DIR)/hash
LIB_MARIO_DIR = $(LIBS_DIR)/mario

CFLAGS += $(LIB_INC)

LIB_GRAPH_OBJS =	$(LIB_GRAPH_DIR)/src/graph.o \
	$(LIB_GRAPH_DIR)/src/font.o \
	$(LIB_GRAPH_DIR)/src/font4x6.o \
	$(LIB_GRAPH_DIR)/src/font5x12.o \
	$(LIB_GRAPH_DIR)/src/font6x8.o \
	$(LIB_GRAPH_DIR)/src/font7x9.o \
	$(LIB_GRAPH_DIR)/src/font8x8.o \
	$(LIB_GRAPH_DIR)/src/font9x8.o \
	$(LIB_GRAPH_DIR)/src/font8x10.o \
	$(LIB_GRAPH_DIR)/src/font8x16.o \
	$(LIB_GRAPH_DIR)/src/font9x16.o \
	$(LIB_GRAPH_DIR)/src/font12x16.o \
	$(LIB_GRAPH_DIR)/src/font10x20.o \
	$(LIB_GRAPH_DIR)/src/font12x24.o \
	$(LIB_GRAPH_DIR)/src/font16x32.o

LIB_X_OBJS = $(LIB_X_DIR)/src/xclient.o

LIB_EXT2_OBJS = $(LIB_EXT2_DIR)/src/ext2fs.o

LIB_CONSOLE_OBJS = $(LIB_CONSOLE_DIR)/src/console.o

LIB_UPNG_OBJS = $(LIB_UPNG_DIR)/src/upng.o

LIB_TGA_OBJS = $(LIB_TGA_DIR)/src/tga.o

LIB_HASH_OBJS = $(LIB_HASH_DIR)/src/hashmap.o

LIB_SCONF_OBJS = $(LIB_SCONF_DIR)/src/sconf.o

LIB_M_OBJS = $(LIB_M_DIR)/src/mstr.o \
	$(LIB_M_DIR)/src/mstrx.o \
	$(LIB_M_DIR)/src/marray.o \
	$(LIB_M_DIR)/src/mutf8.o

LIB_MARIO_OBJS = $(LIB_MARIO_DIR)/src/mario_bc.o \
	$(LIB_MARIO_DIR)/src/mario_json.o \
	$(LIB_MARIO_DIR)/src/mario_lex.o \
	$(LIB_MARIO_DIR)/src/mario_vm.o \
	$(LIB_MARIO_DIR)/src/compiler.o

LIB_SYS_OBJS = $(LIB_SYS_DIR)/src/syscall.o \
	$(LIB_SYS_DIR)/src/cmain.o \
	$(LIB_SYS_DIR)/src/kserv.o \
	$(LIB_SYS_DIR)/src/charbuf.o \
	$(LIB_SYS_DIR)/src/vfs.o \
	$(LIB_SYS_DIR)/src/ipc.o \
	$(LIB_SYS_DIR)/src/vdevice.o \
	$(LIB_SYS_DIR)/src/proto.o \
	$(LIB_SYS_DIR)/src/thread.o \
	$(LIB_SYS_DIR)/src/basic_math.o \
	$(LIB_SYS_DIR)/src/shm.o \
	$(LIB_SYS_DIR)/src/sd.o \
	$(LIB_SYS_DIR)/src/arch/$(ARCH)/sd_arch.o \
	$(LIB_SYS_DIR)/src/gpio.o \
	$(LIB_SYS_DIR)/src/global.o \
	$(LIB_SYS_DIR)/src/proc.o \
	$(LIB_SYS_DIR)/src/lockc.o \
	$(LIB_SYS_DIR)/src/wait.o \
	$(LIB_SYS_DIR)/src/critical.o \
	$(LIB_SYS_DIR)/src/md5.o \
	$(LIB_SYS_DIR)/src/buffer.o \
	$(LIB_SYS_DIR)/src/mmio.o 

LIB_LIBC_OBJS = $(LIB_LIBC_DIR)/src/unistd.o \
	$(LIB_LIBC_DIR)/src/stdlib.o \
	$(LIB_LIBC_DIR)/src/fcntl.o \
	$(LIB_LIBC_DIR)/src/vprintf.o \
	$(LIB_LIBC_DIR)/src/pthread.o \
	$(LIB_LIBC_DIR)/src/dirent.o \
	$(LIB_LIBC_DIR)/src/stdio.o

LIB_OBJS = $(LIB_SYS_OBJS) \
	$(LIB_SCONF_OBJS) \
	$(LIB_LIBC_OBJS) \
	$(LIB_GRAPH_OBJS) \
	$(LIB_EXT2_OBJS) \
	$(LIB_CONSOLE_OBJS) \
	$(LIB_UPNG_OBJS) \
	$(LIB_TGA_OBJS) \
	$(LIB_HASH_OBJS) \
	$(LIB_M_OBJS) \
	$(LIB_MARIO_OBJS) \
	$(LIB_X_OBJS)

CLEAN = $(LIB_OBJS)

SD = $(BUILD_DIR)/sd.ext2

all: mkdirs libs builtin_apps extra_apps #extra_devs
	@echo "all done."

KERNEL_H = \
	$(KERNEL_DIR)/include/usinterrupt.h \
	$(KERNEL_DIR)/include/syscalls.h \
	$(KERNEL_DIR)/include/fbinfo.h

libs: $(LIB_OBJS)
	$(AR) rT $(TARGET_DIR)/lib/libewokc.a $(LIB_SYS_OBJS)  $(LIB_LIBC_OBJS) $(LIB_M_OBJS)
	@cp -r $(LIB_SYS_DIR)/include/* $(LIB_LIBC_DIR)/include/* $(LIB_M_DIR)/include/* $(TARGET_DIR)/include/
	$(AR) rT $(TARGET_DIR)/lib/libsconf.a $(LIB_SCONF_OBJS)
	@cp -r $(LIB_SCONF_DIR)/include/* $(TARGET_DIR)/include/
	$(AR) rT $(TARGET_DIR)/lib/libgraph.a $(LIB_GRAPH_OBJS)
	@cp -r $(LIB_GRAPH_DIR)/include/* $(TARGET_DIR)/include/
	$(AR) rT $(TARGET_DIR)/lib/libext2.a $(LIB_EXT2_OBJS)
	@cp -r $(LIB_EXT2_DIR)/include/* $(TARGET_DIR)/include/
	$(AR) rT $(TARGET_DIR)/lib/libconsole.a $(LIB_CONSOLE_OBJS)
	@cp -r $(LIB_CONSOLE_DIR)/include/* $(TARGET_DIR)/include/
	$(AR) rT $(TARGET_DIR)/lib/libmario.a $(LIB_MARIO_OBJS)
	@cp -r $(LIB_MARIO_DIR)/include/* $(TARGET_DIR)/include/
	$(AR) rT $(TARGET_DIR)/lib/libx.a $(LIB_X_OBJS)
	@cp -r $(LIB_X_DIR)/include/* $(TARGET_DIR)/include/
	$(AR) rT $(TARGET_DIR)/lib/libupng.a $(LIB_UPNG_OBJS)
	@cp -r $(LIB_UPNG_DIR)/include/* $(TARGET_DIR)/include/
	$(AR) rT $(TARGET_DIR)/lib/libtga.a $(LIB_TGA_OBJS)
	@cp -r $(LIB_TGA_DIR)/include/* $(TARGET_DIR)/include/
	$(AR) rT $(TARGET_DIR)/lib/libhash.a $(LIB_HASH_OBJS)
	@cp -r $(LIB_HASH_DIR)/include/* $(TARGET_DIR)/include/
	@cp -r $(KERNEL_H) $(TARGET_DIR)/include/

include $(ROOT_DIR)/sbin/init/build.mk
include $(ROOT_DIR)/sbin/dev/fbd/build.mk
include $(ROOT_DIR)/sbin/dev/nulld/build.mk
include $(ROOT_DIR)/sbin/dev/xserverd/build.mk
include $(ROOT_DIR)/sbin/dev/rootfsd/build.mk
include $(ROOT_DIR)/sbin/vfsd/build.mk
include $(ROOT_DIR)/sbin/procd/build.mk
include $(ROOT_DIR)/sbin/x/xwm/build.mk

include $(ROOT_DIR)/sbin/dev/arch/versatilepb/ps2keybd/build.mk
include $(ROOT_DIR)/sbin/dev/arch/versatilepb/ps2moused/build.mk
include $(ROOT_DIR)/sbin/dev/arch/versatilepb/ttyd/build.mk

include $(ROOT_DIR)/sbin/dev/arch/raspi2/ttyd/build.mk

include $(ROOT_DIR)/sbin/dev/arch/raspi/ttyd/build.mk
include $(ROOT_DIR)/sbin/dev/arch/raspi/actledd/build.mk
include $(ROOT_DIR)/sbin/dev/arch/raspi/gpiod/build.mk
include $(ROOT_DIR)/sbin/dev/arch/raspi/spid/build.mk
include $(ROOT_DIR)/sbin/dev/arch/raspi/lcdhatd/build.mk
include $(ROOT_DIR)/sbin/dev/arch/raspi/joystickd/build.mk

include $(ROOT_DIR)/bin/shell/build.mk
include $(ROOT_DIR)/bin/pwd/build.mk
include $(ROOT_DIR)/bin/cat/build.mk
include $(ROOT_DIR)/bin/ps/build.mk
include $(ROOT_DIR)/bin/echo/build.mk
include $(ROOT_DIR)/bin/ls/build.mk
include $(ROOT_DIR)/bin/mount/build.mk
include $(ROOT_DIR)/bin/uname/build.mk
include $(ROOT_DIR)/bin/grep/build.mk
include $(ROOT_DIR)/bin/kill/build.mk
include $(ROOT_DIR)/bin/mkdir/build.mk
include $(ROOT_DIR)/bin/rm/build.mk
include $(ROOT_DIR)/bin/launcher/build.mk
include $(ROOT_DIR)/bin/session/build.mk

mkdirs:
	@mkdir -p ${TARGET_DIR}/$(ROOT_DIR)/sbin/dev/versatilepb
	@mkdir -p ${TARGET_DIR}/$(ROOT_DIR)/sbin/dev/raspi
	@mkdir -p ${TARGET_DIR}/$(ROOT_DIR)/sbin/dev/raspi2
	@mkdir -p ${TARGET_DIR}/$(ROOT_DIR)/sbin/x
	@mkdir -p ${TARGET_DIR}/$(ROOT_DIR)/bin
	@mkdir -p ${TARGET_DIR}/$(ROOT_DIR)/include
	@mkdir -p ${TARGET_DIR}/$(ROOT_DIR)/lib
	@mkdir -p ${TARGET_DIR}/$(ROOT_DIR)/dev
	@mkdir -p ${BUILD_DIR}/asm
	@cp -r ${ROOT_DIR}/etc ${TARGET_DIR}/$(ROOT_DIR)
	@cp -r ${ROOT_DIR}/data ${TARGET_DIR}/$(ROOT_DIR)

sd:
	@echo "\n====building ext2 format sdcard image===="
#256M ext2 sd
	dd if=/dev/zero of=${SD} bs=1024 count=262144
	mke2fs ${SD}
	mkdir -p tmp
	sudo fuse-ext2 -o force,rw+ ${SD} ./tmp
	sudo cp -r build/rootfs/* ./tmp
	sudo umount ./tmp
	rm -r tmp
	@echo "==== ext2 format sdcard image created  ====\n"

builtin_apps: ${PROGS}

APPS_DIR=extra/apps
extra_apps:
	cd $(APPS_DIR)/test;make
	cd $(APPS_DIR)/dump;make
	cd $(APPS_DIR)/gtest;make
	cd $(APPS_DIR)/snake;make
	cd $(APPS_DIR)/console;make
	cd $(APPS_DIR)/xconsole;make
	cd $(APPS_DIR)/tga;make
	cd $(APPS_DIR)/png;make
	cd $(APPS_DIR)/mario;make

DEVS_DIR=extra/devs
extra_devs:
	cd $(DEVS_DIR)/arch/raspi/usbd;make

KERNEL=$(KERNEL_DIR)/build/mkos.bin
KERNEL_ELF=$(KERNEL_DIR)/build/mkos.elf
include ${ARCH_DIR}/${ARCH}/config.mk

run: 
	qemu-system-arm $(QEMU_FLAGS) -kernel ${KERNEL} -sd ${SD}

debug:
	qemu-system-arm $(QEMU_FLAGS) -gdb tcp::26000 -S -kernel ${KERNEL} -sd ${SD}

gdb: 
	echo "target remote :26000" > /tmp/gdbinit
	#arm-none-eabi-gdb ${KERNEL_ELF} -x /tmp/gdbinit
	gdb-multiarch ${KERNEL_ELF} -x /tmp/gdbinit #for ubuntu 18.04 without arm-none-eabi-gdb

clean:
	rm -f $(CLEAN)
	rm -fr build
	cd $(APPS_DIR)/test;make clean
	cd $(APPS_DIR)/dump;make clean
	cd $(APPS_DIR)/gtest;make clean
	cd $(APPS_DIR)/snake;make clean
	cd $(APPS_DIR)/console;make clean
	cd $(APPS_DIR)/xconsole;make clean
	cd $(APPS_DIR)/tga;make clean
	cd $(APPS_DIR)/png;make clean
	cd $(APPS_DIR)/mario;make clean

OS = mkos

# tools
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
CXX = arm-none-eabi-c++
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump

export QEMU_AUDIO_DRV=none

ifneq ($(MACH),)
ARCH=arm/$(MACH)
else
ARCH=arm/versatilepb
endif

ARCH_DIR=arch
include ${ARCH_DIR}/${ARCH}/config.mk

BUILD_DIR = build
SYS_FULL_DIR = ../system

# flags
#OPTM = -g
OPTM = -O2

CFLAGS += $(OPTM) \
	-I. -Ilib/include -Iinclude -Iextra/include -Iextra/lib/include \
	-I${ARCH_DIR}/${ARCH} \
	-marm $(ARCH_QEMU_CFLAGS) \
	-Wstrict-prototypes \
	-pedantic -Wall -Wextra -msoft-float -fPIC -mapcs-frame \
	-fno-builtin-printf \
	-fno-builtin-strcpy \
	-Wno-overlength-strings \
	-fno-builtin-exit \
	-fno-builtin-stdio \
	-fno-builtin-memset \
	-fno-builtin-memcpy \
	-fno-builtin-strchr \
	-fno-builtin-strcmp \
	-fno-builtin-strlen \
	-fno-builtin-strncpy \
	-fno-builtin-strncmp \
	-std=c99

ARCH_OBJS = \
	${ARCH_DIR}/${ARCH}/boot.o \
	${ARCH_DIR}/arm/common/interrupt.o \
	${ARCH_DIR}/arm/common/system.o \
	${ARCH_DIR}/arm/common/memcpy.o \
	${ARCH_DIR}/${ARCH}/hw_info.o \
	${ARCH_DIR}/${ARCH}/uart.o \
	${ARCH_DIR}/${ARCH}/irq_arch.o \
	${ARCH_DIR}/${ARCH}/actled.o \
	${ARCH_DIR}/${ARCH}/timer.o

ifeq ($(MACH),raspi)
CFLAGS += -Iextra/${ARCH_DIR}/${ARCH} -DWITH_LCDHAT

ARCH_OBJS += ${ARCH_DIR}/${ARCH}/gpio_arch.o  \
	${ARCH_DIR}/${ARCH}/cpu_freq.o \
	${ARCH_DIR}/${ARCH}/mailbox.o \
	extra/${ARCH_DIR}/${ARCH}/lib/spi_arch.o \
	extra/${ARCH_DIR}/${ARCH}/lcdhat/lcdhat.o 
else
ifeq ($(MACH),raspi2)
ARCH_OBJS += ${ARCH_DIR}/${ARCH}/mailbox.o
endif
endif
	
LIB_DIR = lib/src
EXTRA_LIB_DIR = extra/lib/src

LIB_OBJS = \
	${LIB_DIR}/kstring.o \
	${LIB_DIR}/vprintf.o \
	${LIB_DIR}/proto.o \
	${LIB_DIR}/basic_math.o \
	${LIB_DIR}/queue.o \
	${LIB_DIR}/kprintf.o

ifeq ($(TINYSYS),yes)
LOAD_DIR = extra/loadinit/mem
LOAD_OBJS = \
	${LOAD_DIR}/base16.o \
	${LOAD_DIR}/init_data.o \
	${LOAD_DIR}/loadinit.o

else
CFLAGS += -DSDC -DFRAMEBUFFER
LIB_OBJS += \
	${EXTRA_LIB_DIR}/graph.o \
	${EXTRA_LIB_DIR}/font.o \
	${EXTRA_LIB_DIR}/font8x16.o \
	${EXTRA_LIB_DIR}/console.o \
	${EXTRA_LIB_DIR}/kconsole.o

ARCH_OBJS += extra/${ARCH_DIR}/${ARCH}/sd/sd.o \
	extra/${ARCH_DIR}/${ARCH}/framebuffer/framebuffer.o 

LOAD_DIR = extra/loadinit/sd
LOAD_OBJS = \
	${LOAD_DIR}/ext2read.o \
	${LOAD_DIR}/loadinit.o

QEMU_ARGS = -sd $(SYS_FULL_DIR)/build/sd.ext2
endif

CFLAGS += -I$(LOAD_DIR)

KERNEL_DIR = kernel
KERNEL_OBJS = \
	${KERNEL_DIR}/mm/startup.o \
	${KERNEL_DIR}/mm/kalloc.o \
	${KERNEL_DIR}/mm/mmu.o \
	${KERNEL_DIR}/mm/trunkmem.o \
	${KERNEL_DIR}/mm/kmalloc.o \
	${KERNEL_DIR}/mm/shm.o \
	${KERNEL_DIR}/init.o \
	${KERNEL_DIR}/irq.o \
	${KERNEL_DIR}/svc.o \
	${KERNEL_DIR}/proc.o \
	${KERNEL_DIR}/schedule.o \
	${KERNEL_DIR}/systemc.o \
	${KERNEL_DIR}/kevqueue.o \
	${KERNEL_DIR}/kernel.o

OBJS = ${ARCH_OBJS} \
	${LIB_OBJS} \
	${LOAD_OBJS} \
	${KERNEL_OBJS} 

LDS = mkos.lds.sd.S
LDFLAGS = -L ${ARCH_DIR}/${ARCH}

ASFLAGS = $(OPTM) -I. -Iinclude -marm $(ARCH_QEMU_CFLAGS)

all: $(OS).bin 

$(OS).bin: $(OBJS) ${LDS}
	mkdir -p ${BUILD_DIR}
	$(LD) $(OBJS) $(LDFLAGS) -T ${LDS} -o ${BUILD_DIR}/$(OS).elf
	$(OBJCOPY) -O binary ${BUILD_DIR}/$(OS).elf ${BUILD_DIR}/$(OS).bin
#	$(OBJDUMP) -D ${BUILD_DIR}/$(OS).elf > ${BUILD_DIR}/$(OS).asm


run: 
	qemu-system-arm $(QEMU_FLAGS) -kernel ${BUILD_DIR}/$(OS).bin ${QEMU_ARGS}

runasm: 
	qemu-system-arm $(QEMU_FLAGS) -kernel ${BUILD_DIR}/$(OS).bin ${QEMU_ARGS} -d in_asm

debug:
	qemu-system-arm $(QEMU_FLAGS) -gdb tcp::26000 -S -kernel ${BUILD_DIR}/$(OS).bin ${QEMU_ARGS}

debugasm:
	qemu-system-arm $(QEMU_FLAGS) -d in_asm -gdb tcp::26000 -S -kernel ${BUILD_DIR}/$(OS).bin

gdb: 
	echo "target remote :26000" > /tmp/gdbinit
	#arm-none-eabi-gdb ${BUILD_DIR}/$(OS).elf -x /tmp/gdbinit
	gdb-multiarch ${BUILD_DIR}/$(OS).elf -x /tmp/gdbinit #for ubuntu 18.04 without arm-none-eabi-gdb

clean:
	rm -f $(OBJS)
	rm -fr ${BUILD_DIR}

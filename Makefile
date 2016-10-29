#EXTRA_CFLAGS += -mfpu=neon
#EXTRA_CFLAGS += -D__KERNEL__ -DKERNEL -march=armv8-a -mtune=cortex-a9

ARCH		= arm64
KERNEL ?= /home/chaosmaster/htcroot/kernel/542_himauhl-3.10.84-perf-g9fb087d
CONFIG = msm8994_defconfig
CROSS_COMPILE ?= /home/chaosmaster/htcroot/android/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-
LD = $(CROSS_COMPILE)ld
MDIR ?= $(PWD)

CPPFLAGS	=  -I$(KERNEL)/

obj-m += selinux_permissive.o
selinux_permissive-objs := selinux-permissive.o

all: module push

module:
	ARCH=$(ARCH) LD=$(LD) CROSS_COMPILE=$(CROSS_COMPILE) make -C $(KERNEL)/ M=$(MDIR) modules

clean:
	ARCH=$(ARCH) LD=$(LD) CROSS_COMPILE=$(CROSS_COMPILE) make -C $(KERNEL)/ M=$(MDIR) clean
	rm -f *.order

prepare:
	ARCH=$(ARCH) LD=$(LD) CROSS_COMPILE=$(CROSS_COMPILE) make -C $(KERNEL)/ $(CONFIG)
	ARCH=$(ARCH) LD=$(LD) CROSS_COMPILE=$(CROSS_COMPILE) make -C $(KERNEL)/ modules_prepare

kernel:
	ARCH=$(ARCH) LD=$(LD) CROSS_COMPILE=$(CROSS_COMPILE) make -C $(KERNEL)/

kernel_clean:
	ARCH=$(ARCH) LD=$(LD) CROSS_COMPILE=$(CROSS_COMPILE) make -C $(KERNEL)/ mrproper
	ARCH=$(ARCH) LD=$(LD) CROSS_COMPILE=$(CROSS_COMPILE) make -C $(KERNEL)/ clean

push:
	adb push selinux_permissive.ko /data/local/tmp/

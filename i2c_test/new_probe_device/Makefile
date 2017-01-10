
KERNEL_DIR := /lib/modules/3.2.0/build

PLATFORM := "am335x-evm"  
MACHINE_NAME := "am335x"

CROSS_COMPILE=/opt/i686-arago-linux/usr/bin/arm-linux-gnueabihf-
export CROSS_COMPILE

obj-m := test.o

MAKE_ENV = ARCH=arm  

PWD := $(shell pwd)
all:  
	$(MAKE) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" -C $(KERNEL_DIR) $(MAKE_ENV) M=$(PWD) modules


clean:  
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.symvers  





KDIR = /mysoftware/linux-3.2.0-psp04.06.00.11-m3352/

obj-m = test.o

all:
	make CROSS_COMPILE=arm-linux-gnueabihf- ARCH=arm -C $(KDIR) modules M=`pwd` 
	#make -C $(KDIR) modules M=`pwd` 

clean:
	make CROSS_COMPILE=arm-linux-gnueabihf- ARCH=arm -C $(KDIR) modules M=`pwd` clean
	#make -C $(KDIR) modules M=`pwd` clean 


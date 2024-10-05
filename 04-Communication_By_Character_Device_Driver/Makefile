EXTRA_CFLAGS=-Wall
obj-m=kernelspace.o #Khi build exam.c -> exam.ko 

KDIR=/lib/modules/`uname -r`/build 

all:
	make -C $(KDIR) M=`pwd` modules 
	gcc -o userspace userspace.c 
clean:
	make -C $(KDIR) M=`pwd` clean
	rm -rf userspace
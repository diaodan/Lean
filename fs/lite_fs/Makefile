
EXTRA_CFLAGS=-O -g

ifneq ($(KERNELRELEASE),)
	obj-m := lite_fs.o
	lite_fs-objs := lite_super.o lite_inode.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build/
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	rm -f *.ko *.mod.* *.o *.order *.symvers .*.cmd

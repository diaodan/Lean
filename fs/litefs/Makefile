ifneq ($(KERNELRELEASE),)
#litefs-objs := inode.o
	obj-m := litefs.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build/
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	rm -f *.ko *.mod.* *.o *.order *.symvers

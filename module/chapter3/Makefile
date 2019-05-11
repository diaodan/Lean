
ccflags-y+=-O -g

ifneq ($(KERNELRELEASE),)
	obj-m := scull.o
	scull-objs := main.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build/
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	rm -f *.ko *.mod.* *.o *.order *.symvers .*.cmd

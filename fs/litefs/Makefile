EXTRA_CFLAGS := -g3
ifneq ($(KERNELRELEASE),)
	obj-m := litefs.o
#	lite_fs-objs := inode.o litefs.o
else
	KERNELDIR ?= /lib/modules/2.6.32.71/build/
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	rm -rf *.ko *.mod.* *.o *.order *.symvers .*.ko.* .*.mod.* .*.*.cmd .tmp_versions

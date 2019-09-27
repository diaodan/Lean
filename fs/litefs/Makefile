EXTRA_CFLAGS := -g
ifneq ($(KERNELRELEASE),)
	obj-m := lite_fs.o
	lite_fs-objs := inode.o litefs.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build/
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	rm -rf *.ko *.mod.* *.o *.order *.symvers .*.ko.* .*.mod.* .*.*.cmd .tmp_versions

EXTRA_CFLAGS := -g3
ifneq ($(KERNELRELEASE),)
	lite_fs-objs := inode.o litefs.o dir.o file.o namei.o
	obj-m := lite_fs.o
else
	KERNELDIR ?= /lib/modules/`uname -r`/build/
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	rm -rf *.ko *.mod.* *.o *.order *.symvers .*.ko.* .*.mod.* .*.*.cmd .tmp_versions

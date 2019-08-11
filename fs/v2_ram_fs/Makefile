
EXTRA_CFLAGS=-O -g

ifneq ($(KERNELRELEASE),)
	obj-m := v2_ram_fs.o
	v2_ram_fs-objs := v2_super.o v2_inode.o v2_sops.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build/
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	rm -f *.ko *.mod.* *.o *.order *.symvers .*.cmd

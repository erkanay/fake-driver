CONFIG_MODULE_SIG=n

ifneq ($(KERNELRELEASE),)
	obj-m := fake_driver_kernel.o

else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build 
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

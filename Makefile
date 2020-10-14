#KERNELDIR := /home/lty/work/linux_study/linux-5.8.5
######################################################

KERNELDIR := /lib/modules/$(shell uname -r)/build

CURRENT_PATH := $(shell pwd)

obj-m := chrdevbase.o

build : kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean

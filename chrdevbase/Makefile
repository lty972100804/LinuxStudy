KERNEL_DIR := /lib/modules/$(shell uname -r)/build

CURRENT_PATH := $(shell pwd)

obj-m := chrdevbase.o

build: kernel_modules

kernel_modules:
	@echo 正在编译模块
	$(MAKE) -C $(KERNEL_DIR) M=$(CURRENT_PATH) modules

clean:
	@echo 正在清理中间文件...
	$(MAKE) -C $(KERNEL_DIR) M=$(CURRENT_PATH) clean
	@echo 中间文件清理完成


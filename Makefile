obj-m  = exec_cmd.o
SLES   = "sles"
UBUNTU = ubuntu

OS = $(shell grep "^ID=" /etc/os-release|cut -d"=" -f2)

ifeq ($(OS), $(SLES))
	KERNEL_SRC_DIR    = /usr/src/linux-$(shell uname -r|sed -e "s/-default//g")-obj/x86_64/default
	KERNEL_HEADER_DIR = /usr/src/linux-$(shell uname -r|sed -e "s/-default//g")/include
	BUILD_DIR         = $(shell pwd)
	EXTRA_CFLAGS     += -I$(KERNEL_HEADER_DIR) -g -Werror
endif
ifeq ($(OS), $(UBUNTU))
	KERNEL_SRC_DIR    = /lib/modules/$(shell uname -r)/build
	BUILD_DIR         = $(shell pwd)
endif

default:
	$(MAKE) -C $(KERNEL_SRC_DIR) M=$(BUILD_DIR) modules
clean:
	rm  -rf *.o .exec_cmd.* *.ko* *~ modules.order exec_cmd.mod.c Module.symvers .tmp_versions
test:
	@echo OS = $(OS)
	@echo KERNEL_SRC_DIR = $(KERNEL_SRC_DIR)

obj-m = exec_cmd.o


KERNEL_SRC_DIR = /usr/src/linux-$(shell uname -r|sed -e "s/-default//g")-obj/x86_64/default
KERNEL_HEADER_DIR = /usr/src/linux-$(shell uname -r|sed -e "s/-default//g")/include
BUILD_DIR      = $(shell pwd)
EXTRA_CFLAGS += -I$(KERNEL_HEADER_DIR) -g -Werror
default:
	$(MAKE) -C $(KERNEL_SRC_DIR) M=$(BUILD_DIR) modules
clean:
	rm  -rf *.o .exec_cmd.* *.ko* *~ modules.order exec_cmd.mod.c Module.symvers .tmp_versions

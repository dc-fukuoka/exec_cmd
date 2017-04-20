obj-m = exec_cmd.o

KERNEL_SRC_DIR = /usr/src/kernels/$(shell uname -r)
BUILD_DIR      = $(shell pwd)
EXTRA_CFLAGS += -g -Werror
default:
	$(MAKE) -C $(KERNEL_SRC_DIR) M=$(BUILD_DIR) modules
clean:
	rm  -rf *.o .exec_cmd.* *.ko* *~ modules.order exec_cmd.mod.c Module.symvers .tmp_versions

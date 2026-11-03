# Builds elevator.ko in this folder from the source in src/.
obj-m := elevator.o
elevator-y := src/main.o src/requests.o src/scheduler.o src/proc.o src/pets.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

.PHONY: all clean


ifneq   "$(findstring 2.4.,$(shell uname -a))" ""

all:
	make -f Makefile-2.4

clean:
	make -f Makefile-2.4 clean

install:
	make -f Makefile-2.4 install

else

KERNELVER := $(shell uname -r)
EXTRA_CFLAGS += -O2
EXTRA_CFLAGS += -fno-stack-protector
EXTRA_CFLAGS += -I../../include

ifneq ($(KERNELRELEASE),)

obj-m := ipcdrv.o
ipcdrv-objs := ipcext.o ipcproc.o ioctlrw.o ipcmodule.o ipcsem.o ipcmutex.o ipcevent.o ipcshm.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" modules
	cp *.ko ../../lib

endif

distclean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.bak .*.cache *.d
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.bak .*.cache *.d *.markers *.symvers *.order .cache.mk

depend .depend dep:
	$(CC) $(EXTRA_CFLAGS) -M *.c > .depend

ifeq (.depend, $(wildcard .depend))
include .depend
endif

ifeq "$(INSTALL_PREFIX)" ""
install:
	chmod +x $(PWD)/insert
	$(PWD)/insert
else
install:
	cp ipcdrv.ko $(INSTALL_PREFIX)/home/$(TARGETFS_USER)/drivers/
endif

endif

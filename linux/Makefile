
all:
	make -f Makefile.static
	make -f Makefile.dynamic

clean:
	make -f Makefile.static clean
	make -f Makefile.dynamic clean
	make -f Makefile.dynamic ipcdrv.clean

install:
	make -f Makefile.static install.static
	make -f Makefile.dynamic install.dynamic

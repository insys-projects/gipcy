
ROOT_DIR = .

BASENAME = libgipcy.so
LIBNAME  = $(BASENAME).0.0
SONAME   = $(BASENAME).0
TARGET_NAME = $(LIBNAME)

all: $(LIBNAME)

include ../Mk.platform
include ./Mk.Rules

ifeq ($(IPC_TYPE),_POSIX_IPC_)
    DIRS := . ./posix ../include
endif
ifeq ($(IPC_TYPE),_INSYS_IPC_)
    DRV := ipcdrv.module
    DIRS := . ./insys ./ipcdrv ../include
endif

INC := $(addprefix  -I, $(DIRS))

#CFLAGS += -D__VERBOSE__
CFLAGS += -D$(IPC) $(INC)

EXTFILES :=

SRC_DIR := 	$(DIRS)
SRC_FILES := 	$(addsuffix /*.cpp, $(SRC_DIR))
SRC := 		$(wildcard $(SRC_FILES))
OBJ_FILES := 		$(patsubst %.cpp,%.o, $(SRC) $(EXTFILES))
LIBDIR := ../lib

$(LIBNAME): $(OBJ_FILES) 
	$(LD) -g -shared -fPIC -Wl,-soname,$(SONAME) \
        -o $(LIBNAME) $(notdir $(OBJ_FILES))
	chmod 666 $(LIBNAME)
	ln -sf $(LIBNAME) $(SONAME)
	ln -sf $(SONAME) $(BASENAME)
	
	cp -d $(LIBNAME) $(LIBDIR)
	cp -d $(SONAME) $(LIBDIR)
	cp -d $(BASENAME) $(LIBDIR)
	
	cp -d $(LIBNAME) ../../bin
	cp -d $(SONAME) ../../bin
	cp -d $(BASENAME) ../../bin

test:
	@echo $(SRC_DIR)
	@echo $(SRC_FILES)
	@echo $(SRC)
	@echo $(OBJ_FILES)

ipcdrv.module:
	@echo Build ipcdrv kernel module
	make -C ./ipcdrv

ipcdrv.clean:
	@echo Clean ipcdrv kernel module
	make -C ./ipcdrv clean
	rm -fv ../lib/*.so ../lib/*.so.0 ../lib/*.so.0.0
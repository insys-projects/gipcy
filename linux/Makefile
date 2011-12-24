
ROOT_DIR = .

BASENAME = libgipcy.so
LIBNAME  = $(BASENAME).0.0
SONAME   = $(BASENAME).0

all: $(LIBNAME)

DIRS := ../include .

INC := $(addprefix  -I, $(DIRS))

#CFLAGS += -D__IPC_LINUX__
#CFLAGS += -D__VERBOSE__
CFLAGS += $(INC)

EXTFILES :=

SRC_DIR := 	$(DIRS)
SRC_FILES := 	$(addsuffix /*.cpp, $(SRC_DIR))
SRC := 		$(wildcard $(SRC_FILES))
OBJ := 		$(patsubst %.cpp,%.o, $(SRC) $(EXTFILES))
LIBDIR := ../lib

$(LIBNAME): $(OBJ)
	$(LD) -g -shared -rdynamic -Wl,-soname,$(SONAME) \
	-o $(LIBNAME) $(notdir $(OBJ)) -ldl -lpthread -lc -lrt
	chmod 666 $(LIBNAME)
	ln -sf $(LIBNAME) $(SONAME)
	ln -sf $(SONAME) $(BASENAME)
	
	cp -d $(LIBNAME) $(LIBDIR)
	cp -d $(SONAME) $(LIBDIR)
	cp -d $(BASENAME) $(LIBDIR)
	
	cp -d $(LIBNAME) ../../bin
	cp -d $(SONAME) ../../bin
	cp -d $(BASENAME) ../../bin

include ./Mk.Rules

test:
	@echo $(SRC_DIR)
	@echo $(SRC_FILES)
	@echo $(SRC)

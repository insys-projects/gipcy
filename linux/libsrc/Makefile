
LIBNAME  := libgipcy.so
SONAME   := $(LIBNAME).0
LIBDIR   := ../../lib

include ../../make/macros.mk

DIRS += . ../../include
ifeq ($(IPC_TYPE), _SYSTEMV_IPC_)
    IPCDIR := ./systemv
endif
ifeq ($(IPC_TYPE), _POSIX_IPC_)
    IPCDIR := ./posix
endif
ifeq ($(IPC_TYPE), _INSYS_IPC_)
    IPCDIR := ./insys
    DRVDIR := ../ipcdrv
endif
ifeq ("$(IPC_TYPE)","")
    $(error You must define IPC_TYPE variable in macros.mk!)
endif
DIRS += $(IPCDIR)
INC := $(addprefix -I, $(DIRS) $(DRVDIR))

EXTFILES := $(wildcard $(IPCDIR)/*.cpp)

SRC := $(wildcard *.cpp)
SRC += $(EXTFILES)
SOURCE += $(SRC)
OBJ_FILES := $(notdir $(SOURCE:.cpp=.o))

CFLAGS += $(INC)
CFLAGS += -fPIC
CFLAGS += -std=c++11
#CFLAGS += -std=gnu++0x

all: $(LIBNAME)

include ../../make/rules.mk
include ../../make/shared.mk
include ../../make/clean.mk

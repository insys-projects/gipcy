#
#change this makefile for your target...
#

TARGET_NAME := ipcdrv

all: $(TARGET_NAME)

DIRS := . ../../include

INC := $(addprefix  -I, $(DIRS))
SRC := $(wildcard *.cpp)
OBJ := $(patsubst %.cpp,%.o, $(SRC))

$(TARGET_NAME): $(OBJ)
	$(LD) -o $(TARGET_NAME) $(notdir $^) $(LFLAGS)
	rm -f *.o *~ core

include ../../Mk.platform
include ../Mk.Rules
include $(wildcard *.d)
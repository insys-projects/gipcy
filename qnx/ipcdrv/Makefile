#
#change this makefile for your target...
#

TARGET_NAME := ipcdrv

all: $(TARGET_NAME)

DIRS := . ../../include

INC := $(addprefix  -I, $(DIRS))

CFLAGS := -g -Wall -D__IPC_QNX__ $(INC)
LFLAGS := -lstdc++

SRC := $(wildcard *.cpp)
OBJ := $(patsubst %.cpp,%.o, $(SRC))

$(TARGET_NAME): $(OBJ)
	$(LD) -o $(TARGET_NAME) $(notdir $^) $(LFLAGS)
	rm -f *.o *~ core

include $(wildcard *.d)

include ../Mk.Rules
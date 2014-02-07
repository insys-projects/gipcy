#
#change this makefile for your target...
#

.PHONY = clean
TARGET_NAME := thread_test


all: $(TARGET_NAME)

LIBPATH := ../../lib
DIRS := ../../include

INC := $(addprefix  -I, $(DIRS))

include ../../Mk.platform
include ../../$(TARGET_OS)/Mk.Rules

CFLAGS += -D$(IPC) $(INC)
LFLAGS += -Wl,-rpath $(LIBPATH) -L"$(LIBPATH)" -lgipcy -lstdc++ -lc

$(TARGET_NAME): $(patsubst %.cpp,%.o, $(wildcard *.cpp))
	$(LD) -o $(TARGET_NAME) $(notdir $^) $(LFLAGS)
	rm -f *.o *~ core

include $(wildcard *.d)


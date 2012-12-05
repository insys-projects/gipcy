#
#change this makefile for your target...
#

.PHONY = clean
TARGET_NAME := sem_test


all: $(TARGET_NAME)

LIBPATH := ../../lib
DIRS := ../../include

INC := $(addprefix  -I, $(DIRS))

CC = g++
LD = g++

CFLAGS = -g -pg -Wall -D__IPC_LINUX__ $(INC)
LFLAGS = -Wl,-rpath $(LIBPATH) -L"$(LIBPATH)" -lgipcy

$(TARGET_NAME): $(patsubst %.cpp,%.o, $(wildcard *.cpp))
	$(LD) -o $(TARGET_NAME) $(notdir $^) $(LFLAGS)
	rm -f *.o *~ core

%.o: %.cpp
	$(CC) $(CFLAGS) -c -MD $<
	
include $(wildcard *.d)


clean:
	rm -f *.o *~ core
	rm -f *.d *~ core
	rm -f $(TARGET_NAME)

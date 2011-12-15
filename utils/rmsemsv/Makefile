#
#change this makefile for your target...
#

PHONY = clean
TARGET_NAME = rmsemsv

all: $(TARGET_NAME)

ROOT_DIR = $(shell pwd)

DIR0 = ../../include

CC = g++
LD = g++

CFLAGS := -D__LINUX__ -D__LINUX_IPC__ -g -Wall -I $(DIR0)

$(TARGET_NAME): $(patsubst %.cpp,%.o, $(wildcard *.cpp))
	$(LD) $(LFLAGS) $^ -o $(TARGET_NAME)
	cp $(TARGET_NAME) ../bin
	rm -f *.o *~ core

%.o: %.cpp
	$(CC) $(CFLAGS) -c -MD $<
	
include $(wildcard *.d)


clean:
	rm -f *.o *~ core
	rm -f *.d *~ core
	rm -f $(TARGET_NAME)
	
distclean:
	rm -f *.o *~ core
	rm -f *.d *~ core
	rm -f $(TARGET_NAME)

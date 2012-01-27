#
#change this makefile for your target...
#

PHONY = clean
TARGET_NAME1 := sem_test
TARGET_NAME2 := mutex_test
TARGET_NAME3 := event_test
TARGET_NAME4 := dll_test
TARGET_NAME5 := shm_test
TARGET_NAME6 := kb_test

all: $(TARGET_NAME1) $(TARGET_NAME2) $(TARGET_NAME3) $(TARGET_NAME4) $(TARGET_NAME5) $(TARGET_NAME6)

LIBPATH := ../lib
DIRS := ../include

INC := $(addprefix  -I, $(DIRS))

CC = g++
LD = g++

CFLAGS = -g -Wall -D__IPC_LINUX__ $(INC)
LFLAGS = -Wl,-rpath $(LIBPATH) -L"$(LIBPATH)" -lgipcy

$(TARGET_NAME1): $(patsubst %.cpp,%.o, sem_test.cpp)
	$(LD) -o $(TARGET_NAME1) $(notdir $^) $(LFLAGS)

$(TARGET_NAME2): $(patsubst %.cpp,%.o, mutex_test.cpp)
	$(LD) -o $(TARGET_NAME2) $(notdir $^) $(LFLAGS)

$(TARGET_NAME3): $(patsubst %.cpp,%.o, event_test.cpp)
	$(LD) -o $(TARGET_NAME3) $(notdir $^) $(LFLAGS)

$(TARGET_NAME4): $(patsubst %.cpp,%.o, dll_test.cpp)
	$(LD) -o $(TARGET_NAME4) $(notdir $^) $(LFLAGS)

$(TARGET_NAME5): $(patsubst %.cpp,%.o, shm_test.cpp)
	$(LD) -o $(TARGET_NAME5) $(notdir $^) $(LFLAGS)

$(TARGET_NAME6): $(patsubst %.cpp,%.o, kb_test.cpp)
	$(LD) -o $(TARGET_NAME6) $(notdir $^) $(LFLAGS)

	rm -f *.o *~ core

%.o: %.cpp
	$(CC) $(CFLAGS) -c -MD $<
	
include $(wildcard *.d)


clean:
	rm -f *.o *~ core
	rm -f *.d *~ core
	rm -f $(TARGET_NAME1) $(TARGET_NAME2) \
	$(TARGET_NAME3) $(TARGET_NAME4) $(TARGET_NAME5) $(TARGET_NAME6)
	
distclean:
	rm -f *.o *~ core
	rm -f *.d *~ core
	rm -f $(TARGET_NAME1) $(TARGET_NAME2) \
	$(TARGET_NAME3) $(TARGET_NAME4) $(TARGET_NAME5) $(TARGET_NAME6)

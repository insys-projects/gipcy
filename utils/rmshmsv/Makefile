
.PHONY = clean
TARGET_NAME := rmshmsv

all: $(TARGET_NAME)

LIBPATH := ../../lib
DIRS := ../../include
INC := $(addprefix  -I, $(DIRS))

LIBS += -lstdc++
CFLAGS += -D$(IPC) $(INC)
LFLAGS += -Wl,-rpath $(LIBPATH) -L"$(LIBPATH)" $(LIBS)

$(TARGET_NAME): $(patsubst %.cpp,%.o, $(wildcard *.cpp))
	$(LD) -o $(TARGET_NAME) $(notdir $^) $(LFLAGS)
	cp $(TARGET_NAME) ../bin

include ../../make/macros.mk
include ../../make/rules.mk
include ../../make/clean.mk


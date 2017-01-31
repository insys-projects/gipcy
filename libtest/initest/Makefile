
.PHONY = clean
TARGET_NAME := ini_test

all: $(TARGET_NAME)

LIBPATH := ../../lib
DIRS := ../../include
INC := $(addprefix  -I, $(DIRS))

LIBS += -lgipcy -lstdc++ -lpthread -ldl
CFLAGS += -D$(IPC) $(INC)
LFLAGS += -Wl,-rpath $(LIBPATH) -L"$(LIBPATH)" $(LIBS)

$(TARGET_NAME): $(patsubst %.cpp,%.o, $(wildcard *.cpp))
	$(LD) -o $(TARGET_NAME) $(notdir $^) $(LFLAGS)

include ../../make/macros.mk
include ../../make/rules.mk
include ../../make/clean.mk


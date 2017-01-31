#
# Переменная DIRS содержит
# пути для поиска исходников
# и заголовочных файлов. Задается
# в Makefile нижнего уровня
#
VPATH := $(DIRS)

CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)gcc

CFLAGS += -Wall -g $(TARGET) -D$(IPC) -D$(IPC_TYPE) -D$(OS) -D$(DEF_TYPES) $(GPROF) $(DZY_VER)
CXXFLAGS += $(CFLAGS)

%.o: %.cpp
	$(CC) $(CXXFLAGS) -c -MMD $<

%.o: %.c
	$(CC) $(CFLAGS) -c -MMD $<

include $(wildcard *.d)
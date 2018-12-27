#
# Общие правила для сборки разделяемых библиотек в BARDY
# Файл shared.mk должен быть включен в Makefile собираемой
# библиотеки. В Makefile библиотеки необходимо определить
# следующие переменные:
#
# OBJ_FILES - список объектных файлов для линковки библиотеки
# LIBNAME := libbrd.so - имя собираемой библиотеки
# SONAME   := $(LIBNAME).0 - имя символьной ссылки на библиотеку
# LIBDIR - директория для установки полученной библиотеки
#

LIBS += -lc -lrt -ldl -lpthread
LDOPTIONS := -Wl,-rpath $(LIBDIR) -L"$(GIPCYLIB)" $(LIBS)

# OS dependent flags
RDYNAMIC := -rdynamic
SHARED := -shared
DEBUG := -g
LDFLAGS := $(DEBUG) $(SHARED) $(RDYNAMIC)

$(LIBNAME): $(OBJ_FILES)
	$(LD) $(LDFLAGS) -Wl,-soname,$(SONAME) \
	-o $(LIBNAME) $(notdir $(OBJ_FILES)) $(LDOPTIONS)
	chmod 666 $(LIBNAME)
	ln -sf $(LIBNAME) $(SONAME)
	cp -d $(LIBNAME) $(LIBDIR)
	cp -d $(SONAME) $(LIBDIR)

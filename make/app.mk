#
# Общие правила для сборки приложений в GIPCY
# Файл app.mk должен быть включен в Makefile собираемого
# приложения. В Makefile приложения необходимо определить
# следующие переменные:
#
# OBJ_FILES - список объектных файлов для линковки приложения
# TARGET_NAME := deasy - имя собираемого приложения
# GIPCYLIB - директория с библиотекой libgipcy.so
#

LIBS += -lgipcy  -lstdc++ -ldl -lpthread -lc -lrt -lm
LDOPTIONS := -Wl,-rpath,$(GIPCYLIB) -L"$(GIPCYLIB)" $(LIBS)

$(TARGET_NAME): $(OBJ_FILES)
	$(LD) -o $(TARGET_NAME) $(notdir $(OBJ_FILES)) $(LDOPTIONS)

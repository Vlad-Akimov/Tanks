# Компилятор и флаги
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -g -I src
LDFLAGS = -pthread

# Директории
SRCDIR = src
OBJDIR = obj
BINDIR = bin
DEPDIR = $(OBJDIR)/.deps

# Поиск всех исходных файлов
SOURCES := $(shell find $(SRCDIR) -name '*.cpp')
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/tanks_game

# Флаги для генерации зависимостей
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

# Основная цель
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Сборка завершена: $(TARGET)"

# Компиляция с учетом зависимостей
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(DEPDIR)
	@mkdir -p $(dir $@)
	$(CXX) $(DEPFLAGS) $(CXXFLAGS) -c $< -o $@
	@mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

# Создание директорий
$(BINDIR) $(OBJDIR) $(DEPDIR):
	@mkdir -p $@

# Включение файлов зависимостей
include $(wildcard $(DEPDIR)/*.d)

# Цели
.PHONY: all run clean debug release install uninstall

all: $(TARGET)

run: $(TARGET)
	@echo "Запуск игры..."
	./$(TARGET)

debug: CXXFLAGS += -DDEBUG -O0 -ggdb3
debug: $(TARGET)

release: CXXFLAGS += -O3 -DNDEBUG
release: LDFLAGS += -s  # Убираем символы в релизе
release: $(TARGET)

clean:
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "Очистка завершена"

distclean: clean
	rm -f *~ .*~

info:
	@echo "=== Информация о проекте ==="
	@echo "Компилятор: $(CXX)"
	@echo "Флаги компиляции: $(CXXFLAGS)"
	@echo "Флаги линковки: $(LDFLAGS)"
	@echo "Исходники: $(words $(SOURCES)) файлов"
	@echo "Цель: $(TARGET)"
	@echo "============================="

help:
	@echo "Доступные цели:"
	@echo "  all      - сборка проекта (по умолчанию)"
	@echo "  run      - сборка и запуск"
	@echo "  debug    - отладочная сборка"
	@echo "  release  - релизная сборка"
	@echo "  clean    - очистка"
	@echo "  info     - информация о проекте"
	@echo "  help     - эта справка"
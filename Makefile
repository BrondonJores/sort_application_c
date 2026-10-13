CC = gcc

TARGET = sorting_benchmark
MAIN = main.c

SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS = -I$(INCLUDE_DIR) $(addprefix -I,$(shell find $(SRC_DIR) -type d))

Recherche récursive des fichiers C dans src/

SOURCES = $(MAIN) $(shell find $(SRC_DIR) -type f -name ‘*.c’)

Conservation de l’arborescence dans build/

OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS = $(OBJECTS:.o=.d)

.PHONY: all clean debug

all: $(TARGET)

Édition des liens

$(TARGET): $(OBJECTS)
$(CC) $(OBJECTS) -o $@

Compilation de chaque fichier C

$(BUILD_DIR)/%.o: %.c
mkdir -p $(dir $@)
$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

Mode débogage

debug: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g -O0
debug: clean all

Nettoyage

clean:
rm -rf $(BUILD_DIR) $(TARGET)

-include $(DEPS)
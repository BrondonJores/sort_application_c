CC = gcc

TARGET = sorting_benchmark
TEST_TARGET = test_runner
MAIN = main.c
TEST_MAIN = tests/test_main.c

SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS = -I$(INCLUDE_DIR) $(addprefix -I,$(shell find $(SRC_DIR) -type d))
LDFLAGS = -lm

# Recherche récursive des fichiers C dans src/
SOURCES = $(MAIN) $(shell find $(SRC_DIR) -type f -name '*.c')
MODULE_SOURCES = $(shell find $(SRC_DIR) -type f -name '*.c')

# Conservation de l'arborescence dans build/
OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))
MODULE_OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(MODULE_SOURCES))
TEST_OBJECTS = $(BUILD_DIR)/tests/test_main.o $(MODULE_OBJECTS)

DEPS = $(OBJECTS:.o=.d) $(BUILD_DIR)/tests/test_main.d

.PHONY: all clean debug test

all: $(TARGET)

# Édition des liens
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Target pour les tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) -o $@ $(LDFLAGS)

# Compilation de chaque fichier C
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

# Mode débogage
debug: CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g -O0
debug: clean all

# Nettoyage
clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET) output

-include $(DEPS)

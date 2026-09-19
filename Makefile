CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Iinclude -g -O0
LDFLAGS :=

SRC_DIR   := src
BUILD_DIR := build
TEST_DIR  := tests

TARGET  := $(BUILD_DIR)/server
SRCS    := $(wildcard $(SRC_DIR)/*.c)
OBJS    := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS    := $(OBJS:.o=.d)

TEST_SRCS    := $(wildcard $(TEST_DIR)/*.c)
TEST_TARGETS := $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%,$(TEST_SRCS))
LIB_OBJS     := $(filter-out $(BUILD_DIR)/main.o,$(OBJS))

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%: $(TEST_DIR)/%.c $(LIB_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< $(LIB_OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGETS)
	@for t in $(TEST_TARGETS); do echo "== $$t"; ./$$t || exit 1; done

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
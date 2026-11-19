-include config.mk

ifeq ($(origin CC),default)
    CC = gcc
endif

CFLAGS   = -std=c11 -Wall -Wextra -Wpedantic -O2
DEPFLAGS = -MMD -MP

SRC_DIR   = src
BUILD_DIR = build
SOURCES   = $(wildcard $(SRC_DIR)/*.c)
OBJECTS   = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS      = $(OBJECTS:.o=.d)

ifeq ($(OS),Windows_NT)
    RAYLIB_PATH ?= C:/raylib/raylib
    INCLUDES = -I$(RAYLIB_PATH)/include -Iinclude
    LIBS     = -L$(RAYLIB_PATH)/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET   = $(BUILD_DIR)/teapruma.exe
    RUN_CMD  = $(TARGET)
else
    RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
    RAYLIB_LIBS   := $(shell pkg-config --libs raylib 2>/dev/null)
    ifeq ($(RAYLIB_LIBS),)
        RAYLIB_LIBS = -lraylib -lm -lpthread -ldl -lrt -lX11
    endif
    INCLUDES = $(RAYLIB_CFLAGS) -Iinclude
    LIBS     = $(RAYLIB_LIBS)
    TARGET   = $(BUILD_DIR)/teapruma
    RUN_CMD  = ./$(TARGET)
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR)

run: all
	$(RUN_CMD)

.PHONY: all clean run

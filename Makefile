CC = C:/Users/arthu/Downloads/w64devkit/bin/gcc.exe
RAYLIB_PATH = C:/Users/arthu/Downloads/raylib-6.0_win64_mingw-w64/raylib-6.0_win64_mingw-w64

INCLUDES = -I$(RAYLIB_PATH)/include -Iinclude
LIBS = -L$(RAYLIB_PATH)/lib -lraylib -lopengl32 -lgdi32 -lwinmm

SRC_DIR = src
BUILD_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
TARGET = $(BUILD_DIR)/teapruma.exe

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CC) -c $< -o $@ $(INCLUDES)

clean:
	del /Q $(BUILD_DIR)\*.o $(BUILD_DIR)\*.exe 2>nul || true

run: all
	$(TARGET)

.PHONY: all clean run
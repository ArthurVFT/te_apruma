# Makefile - compila o jogo "Te Apruma!".
#
#   make        -> gera build/teapruma.exe
#   make run    -> compila (se preciso) e executa
#   make clean  -> apaga os arquivos gerados
#
# Ajuste CC e RAYLIB_PATH para a instalacao de cada maquina.

CC = C:/Users/arthu/Downloads/w64devkit/bin/gcc.exe
RAYLIB_PATH = C:/Users/arthu/Downloads/raylib-6.0_win64_mingw-w64/raylib-6.0_win64_mingw-w64

INCLUDES = -I$(RAYLIB_PATH)/include -Iinclude
LIBS = -L$(RAYLIB_PATH)/lib -lraylib -lopengl32 -lgdi32 -lwinmm

SRC_DIR = src
BUILD_DIR = build

# Pega todos os .c de src/ automaticamente.
SOURCES = $(wildcard $(SRC_DIR)/*.c)
# Troca "src/xxx.c" por "build/xxx.o".
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

all: $(BUILD_DIR)/teapruma.exe

$(BUILD_DIR)/teapruma.exe: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) -c $< -o $@ $(INCLUDES)

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.exe

run: all
	$(BUILD_DIR)/teapruma.exe

.PHONY: all clean run

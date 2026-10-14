# ============================================================
# Makefile - automatiza a compilacao do jogo "Te Apruma!"
#
# Como usar (no terminal, dentro desta pasta):
#   make         -> compila e gera build/teapruma.exe
#   make run     -> compila (se preciso) e executa o jogo
#   make clean   -> apaga os arquivos gerados (.o e .exe)
#
# ATENCAO: os caminhos abaixo apontam para a instalacao do autor (Arthur).
# Se outra pessoa do grupo for compilar, precisa ajustar CC e RAYLIB_PATH
# para onde o w64devkit e o raylib estao instalados na maquina dela.
# ============================================================

# CC = compilador C (gcc). Usamos o caminho absoluto para garantir que
# sera usado o gcc do w64devkit (as variaveis abaixo dependem dele).
CC = C:/Users/arthu/Downloads/w64devkit/bin/gcc.exe

# RAYLIB_PATH = pasta onde o raylib foi extraido (contem include/ e lib/).
RAYLIB_PATH = C:/Users/arthu/Downloads/raylib-6.0_win64_mingw-w64/raylib-6.0_win64_mingw-w64

# INCLUDES = pastas onde o compilador procura os arquivos .h.
#   -I<raylib>/include -> acha raylib.h
#   -Iinclude          -> acha game.h, player.h, entity.h, list.h
INCLUDES = -I$(RAYLIB_PATH)/include -Iinclude

# LIBS = como "linkar" o programa:
#   -L... -lraylib     -> a biblioteca raylib
#   -lopengl32         -> API grafica do Windows que o raylib usa
#   -lgdi32            -> recursos de janela/desenho do Windows
#   -lwinmm            -> som/timers do Windows
LIBS = -L$(RAYLIB_PATH)/lib -lraylib -lopengl32 -lgdi32 -lwinmm

# Pastas do projeto: codigo-fonte em src/, saida em build/.
SRC_DIR = src
BUILD_DIR = build

# wildcard: pega TODOS os .c dentro de src/ (nao precisa listar um por um).
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# patsubst: troca "src/xxx.c" por "build/xxx.o".
# Ou seja, para cada fonte teremos um arquivo objeto correspondente.
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Nome do executavel final.
TARGET = $(BUILD_DIR)/teapruma.exe

# "all" e o alvo padrao: quando voce digita apenas "make", ele constroi o TARGET.
all: $(TARGET)

# Regra de LINKAGEM: junta todos os .o num unico .exe.
# O sinal "@" antes do comando esconde o comando no terminal (deixa limpo).
# $@ = o alvo (TARGET); $^ = todas as dependencias (OBJECTS).
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

# Regra de COMPILACAO de um .c para .o.
# $< = a primeira dependencia (o arquivo .c); $@ = o alvo (o arquivo .o).
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) -c $< -o $@ $(INCLUDES)

# "clean" apaga os arquivos gerados (objetos e executavel).
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.exe

# "run" depende de "all": primeiro compila, depois executa o jogo.
run: all
	$(TARGET)

# .PHONY avisa que esses alvos NAO sao arquivos de verdade (sao acoes).
.PHONY: all clean run

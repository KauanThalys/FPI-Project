# Makefile - CONFIGURAÇÃO APENAS PARA WINDOWS (Ambiente MinGW/Cygwin)
# Use este arquivo localmente para compilar.

.PHONY: all clean

# --- Configuração Windows ---
# Nome do executável
TARGET = main.exe

# Caminho da Raylib (SEU CAMINHO)
RAYLIB_PATH = C:/raylibb

# Arquivos de origem (Inclui todos os módulos)
SRCS = main.c screens.c game.c graphics.c input.c resources.c levels.c audio.c

# Flags do Compilador (Caminhos de Inclusão)
CFLAGS = -I$(RAYLIB_PATH)/include

# Flags do Linker (Bibliotecas)
LDFLAGS = -L$(RAYLIB_PATH)/lib -lraylib -lopengl32 -lgdi32 -lwinmm -lshell32

# Compilador
CC = gcc

# --------------------------

# Alvo padrão
all: $(TARGET)

# Compilar o executável
$(TARGET): $(SRCS)
	@echo "Compilando para Windows (Caminho: $(RAYLIB_PATH))..."
	$(CC) -O3 $(SRCS) $(CFLAGS) $(LDFLAGS) -o $(TARGET)
	@echo "✓ Executável criado: $(TARGET)"

# Limpar o projeto
clean:
	@echo "Limpando..."
	del $(TARGET) *.o 2>nul || true
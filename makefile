# Nome do executável
TARGET = Game.exe

# Arquivos de origem
SRCS = main.c player.c boss.c

# Include e Library paths da raylib
RAYLIB_PATH = C:/raylibb
CFLAGS = -I$(RAYLIB_PATH)/include
LDFLAGS = -L$(RAYLIB_PATH)/lib -lraylib -lopengl32 -lgdi32 -lwinmm -lshell32

# Compilador
CC = gcc

# Compilar todos os arquivos de origem
$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) $(LDFLAGS) -o $(TARGET)

# Tarefa para limpar o projeto
clean:
	del $(TARGET) *.o
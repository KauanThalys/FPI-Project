 # Nome do executável
TARGET = main.exe

# Arquivos de origem
SRCS = main.c screens.c game.c graphics.c input.c resources.c levels.c

# Include e Library paths da raylib
RAYLIB_PATH = C:/raylib/raylib
CFLAGS = -I$(RAYLIB_PATH)/src
LDFLAGS = -L$(RAYLIB_PATH)/src -lraylib -lopengl32 -lgdi32 -lwinmm -lshell32

# Compilador
CC = gcc

# Compilar todos os arquivos de origem
$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) $(LDFLAGS) -o $(TARGET)

# Tarefa para limpar o projeto
clean:
	del $(TARGET) *.o
.PHONY: all clean extract-frames

# Detectar o sistema operacional
ifeq ($(OS),Windows_NT)
	DETECTED_OS = Windows
	RM = del
	MKDIR = mkdir
	EXE_EXT = .exe
else
	DETECTED_OS = Linux
	RM = rm -f
	MKDIR = mkdir -p
	EXE_EXT =
endif

# Nome do executável
TARGET = main$(EXE_EXT)

# Arquivos de origem
SRCS = main.c screens.c game.c graphics.c input.c resources.c levels.c audio.c

# Include e Library paths da raylib
ifeq ($(DETECTED_OS),Windows)
	RAYLIB_PATH = C:/raylib/raylib
	CFLAGS = -I$(RAYLIB_PATH)/src
	LDFLAGS = -L$(RAYLIB_PATH)/src -lraylib -lopengl32 -lgdi32 -lwinmm -lshell32
else
	RAYLIB_PATH = /usr/local/include
	CFLAGS = -I$(RAYLIB_PATH)
	LDFLAGS = -lraylib -lm -lpthread -ldl -lGL -lX11
endif

# Compilador
CC = gcc

# Alvo padrão
all: $(TARGET)

# Compilar o executável (rápido, sem extrair frames)
$(TARGET): $(SRCS)
	@echo "Compilando..."
	$(CC) -O3 $(SRCS) $(CFLAGS) $(LDFLAGS) -o $(TARGET)
	@echo "✓ Executável criado: $(TARGET)"

# Extrair frames (comando opcional)
ifeq ($(DETECTED_OS),Windows)
extract-frames:
	@if not exist "assets\frames" $(MKDIR) assets\frames
	@for %%f in (assets\*.mp4 assets\*.avi assets\*.mov) do (
		@if exist "%%f" (
			@echo Extraindo frames de %%~nf...
			@ffmpeg -i "%%f" -q:v 2 "assets\frames\%%~nf_%%04d.png" -y >nul 2>&1
		)
	)
else
extract-frames:
	@$(MKDIR) assets/frames
	@for file in assets/*.mp4 assets/*.avi assets/*.mov; do \
		if [ -f "$$file" ]; then \
			echo "Extraindo frames de $$file..."; \
			ffmpeg -i "$$file" -q:v 2 "assets/frames/$$(basename "$$file")_%04d.png" -y 2>/dev/null || true; \
		fi; \
	done
endif

# Limpar
clean:
	$(RM) $(TARGET) *.o
	@echo "Projeto limpo"
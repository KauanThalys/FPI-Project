#include "resources.h"
#include "raylib.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

static Texture2D g_tileset = {0};
static bool g_tilesetLoaded = false;

// ===== BACKGROUND PARALLAX =====
static Texture2D g_background = {0};
static bool g_backgroundLoaded = false;

// Converte caminhos com "/" para o separador correto do SO
static char* NormalizePath(const char *path) {
    if (!path) return NULL;
    char *normalized = (char *)malloc(strlen(path) + 1);
    if (!normalized) return NULL;
    strcpy(normalized, path);
    
#ifdef _WIN32
    for (int i = 0; normalized[i]; i++) {
        if (normalized[i] == '/') {
            normalized[i] = '\\';
        }
    }
#endif
    
    return normalized;
}

void Resources_LoadLogo(LogoData *logo) {
    if (!logo) return;
    logo->loaded = false;
    const char* logoPaths[] = {"logo.png", "logo.jpg", "assets/logo.png"};
    for (int i = 0; i < 3; i++) {
        char *normalized = NormalizePath(logoPaths[i]);
        if (normalized && FileExists(normalized)) {
            logo->texture = LoadTexture(normalized);
            logo->loaded = true;
            free(normalized);
            break;
        }
        if (normalized) free(normalized);
    }
}

void Resources_UnloadLogo(LogoData *logo) {
    if (!logo) return;
    if (logo->loaded) {
        UnloadTexture(logo->texture);
        logo->loaded = false;
    }
}

bool Resources_LoadTileset(const char *path) {
    if (!path) return false;
    char *normalized = NormalizePath(path);
    if (!normalized) return false;
    if (!FileExists(normalized)) {
        free(normalized);
        return false;
    }
    g_tileset = LoadTexture(normalized);
    g_tilesetLoaded = true;
    free(normalized);
    return true;
}

void Resources_UnloadTileset(void) {
    if (g_tilesetLoaded) {
        UnloadTexture(g_tileset);
        g_tilesetLoaded = false;
        memset(&g_tileset, 0, sizeof(g_tileset));
    }
}

bool Resources_HasTileset(void) {
    return g_tilesetLoaded;
}

Texture2D Resources_GetTileset(void) {
    return g_tileset;
}

// ===== BACKGROUND PARALLAX FUNCTIONS =====
bool Resources_LoadBackground(const char *path) {
    if (!path) {
        TraceLog(LOG_WARNING, "[Background] Path é NULL");
        return false;
    }
    
    char *normalized = NormalizePath(path);
    if (!normalized) {
        TraceLog(LOG_WARNING, "[Background] Falha ao normalizar path");
        return false;
    }
    
    TraceLog(LOG_INFO, "[Background] Tentando carregar: %s", normalized);
    
    if (!FileExists(normalized)) {
        TraceLog(LOG_WARNING, "[Background] Arquivo não encontrado: %s", normalized);
        free(normalized);
        return false;
    }
    
    g_background = LoadTexture(normalized);
    
    if (g_background.width > 0 && g_background.height > 0) {
        g_backgroundLoaded = true;
        TraceLog(LOG_INFO, "[Background] ✓ Carregado com sucesso: %s (%dx%d)", 
                 normalized, g_background.width, g_background.height);
        free(normalized);
        return true;
    }
    
    TraceLog(LOG_ERROR, "[Background] Falha ao carregar textura: %s", normalized);
    free(normalized);
    return false;
}

void Resources_UnloadBackground(void) {
    if (g_backgroundLoaded) {
        UnloadTexture(g_background);
        g_backgroundLoaded = false;
        memset(&g_background, 0, sizeof(g_background));
        TraceLog(LOG_INFO, "[Background] Descarregado");
    }
}

bool Resources_HasBackground(void) {
    return g_backgroundLoaded;
}

Texture2D Resources_GetBackground(void) {
    return g_background;
}

// ===== MENU VIDEO SUPPORT =====
#define MAX_MENU_FRAMES 60
static Texture2D *g_menuFrames = NULL;
static int g_menuFrameCount = 0;
static int g_menuCurrentFrame = 0;
static float g_menuFrameTime = 0.125f/15.0f;
static float g_menuFrameTimer = 0.0f;
static bool g_menuVideoLoaded = false;

bool Resources_LoadMenuVideo(const char *path) {
    if (!path) return false;
    
    char *normalized = NormalizePath(path);
    if (!normalized) return false;
    
    TraceLog(LOG_INFO, "[MenuVideo] Tentando carregar: %s", normalized);
    
    if (!DirectoryExists(normalized)) {
        TraceLog(LOG_WARNING, "[MenuVideo] Diretório não encontrado: %s", normalized);
        free(normalized);
        return false;
    }

    FilePathList list = LoadDirectoryFiles(normalized);
    TraceLog(LOG_INFO, "[MenuVideo] Encontrados %d arquivos em %s", list.count, normalized);
    
    if (list.count <= 0) {
        UnloadDirectoryFiles(list);
        free(normalized);
        return false;
    }

    int max_to_load = list.count > MAX_MENU_FRAMES ? MAX_MENU_FRAMES : list.count;
    g_menuFrames = (Texture2D *)RL_CALLOC(max_to_load, sizeof(Texture2D));
    
    for (int i = 0; i < list.count && g_menuFrameCount < MAX_MENU_FRAMES; i++) {
        const char *p = list.paths[i];
        if (IsFileExtension(p, ".png") || IsFileExtension(p, ".jpg") || IsFileExtension(p, ".jpeg")) {
            g_menuFrames[g_menuFrameCount] = LoadTexture(p);
            if (g_menuFrames[g_menuFrameCount].width > 0) {
                g_menuFrameCount++;
            }
        }
    }
    
    UnloadDirectoryFiles(list);
    free(normalized);
    
    if (g_menuFrameCount > 0) {
        g_menuVideoLoaded = true;
        g_menuCurrentFrame = 0;
        g_menuFrameTimer = 0.0f;
        TraceLog(LOG_INFO, "[MenuVideo] ✓ Carregados %d frames com sucesso", g_menuFrameCount);
        return true;
    }
    
    TraceLog(LOG_WARNING, "[MenuVideo] Nenhum frame PNG/JPG encontrado");
    RL_FREE(g_menuFrames);
    g_menuFrames = NULL;
    return false;
}

void Resources_UnloadMenuVideo(void) {
    if (g_menuVideoLoaded && g_menuFrames) {
        for (int i = 0; i < g_menuFrameCount; i++) {
            UnloadTexture(g_menuFrames[i]);
        }
        free(g_menuFrames);
        g_menuFrames = NULL;
        g_menuFrameCount = 0;
        g_menuCurrentFrame = 0;
        g_menuVideoLoaded = false;
    }
}

bool Resources_HasMenuVideo(void) {
    return g_menuVideoLoaded && (g_menuFrameCount > 0);
}

Texture2D Resources_GetMenuVideoTexture(void) {
    if (!g_menuVideoLoaded || g_menuFrameCount <= 0) return (Texture2D){0};
    if (g_menuCurrentFrame >= g_menuFrameCount) g_menuCurrentFrame = 0;
    return g_menuFrames[g_menuCurrentFrame];
}

void Resources_UpdateMenuVideo(void) {
    if (!g_menuVideoLoaded || g_menuFrameCount <= 0) return;
    
    g_menuFrameTimer += GetFrameTime();
    if (g_menuFrameTimer >= g_menuFrameTime) {
        g_menuCurrentFrame = (g_menuCurrentFrame + 1) % g_menuFrameCount;
        g_menuFrameTimer = 0.0f;
    }
}
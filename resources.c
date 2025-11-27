#include "resources.h"
#include "raylib.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

static Texture2D g_tileset = {0};
static bool g_tilesetLoaded = false;

// Tenta carregar a logo do jogo a partir de caminhos comuns.
// Se encontrar, marca `logo->loaded = true` e mantém a textura em `logo->texture`.
void Resources_LoadLogo(LogoData *logo) {
    if (!logo) return;
    logo->loaded = false;
    const char* logoPaths[] = {"logo.png", "logo.jpg", "assets/logo.png"};
    for (int i = 0; i < 3; i++) {
        if (FileExists(logoPaths[i])) {
            logo->texture = LoadTexture(logoPaths[i]);
            logo->loaded = true;
            break;
        }
    }
}

// Descarrega a textura da logo se ela foi carregada.
// Sempre chame isso no final do jogo para evitar vazamentos de memória.
void Resources_UnloadLogo(LogoData *logo) {
    if (!logo) return;
    if (logo->loaded) {
        UnloadTexture(logo->texture);
        logo->loaded = false;
    }
}

// Tenta carregar um tileset a partir do caminho fornecido.
// Retorna true se carregou com sucesso.
bool Resources_LoadTileset(const char *path) {
    if (!path) return false;
    if (!FileExists(path)) return false;
    g_tileset = LoadTexture(path);
    g_tilesetLoaded = true;
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

// ===== MENU VIDEO SUPPORT (optimized frame sequence loading) =====
#define MAX_MENU_FRAMES 60  // Limita carregamento a 200 frames
static Texture2D *g_menuFrames = NULL;
static int g_menuFrameCount = 0;
static int g_menuCurrentFrame = 0;
static float g_menuFrameTime = 0.125f/15.0f; // 30 FPS
static float g_menuFrameTimer = 0.0f;
static bool g_menuVideoLoaded = false;

bool Resources_LoadMenuVideo(const char *path) {
    if (!path) return false;
    TraceLog(LOG_INFO, TextFormat("[MenuVideo] Tentando carregar: %s", path));
    
    if (!DirectoryExists(path)) {
        TraceLog(LOG_WARNING, TextFormat("[MenuVideo] Diretório não encontrado: %s", path));
        return false;
    }

    FilePathList list = LoadDirectoryFiles(path);
    TraceLog(LOG_INFO, TextFormat("[MenuVideo] Encontrados %d arquivos em %s", list.count, path));
    
    if (list.count <= 0) {
        UnloadDirectoryFiles(list);
        return false;
    }

    // Allocate space for frames (limited to MAX_MENU_FRAMES)
    int max_to_load = list.count > MAX_MENU_FRAMES ? MAX_MENU_FRAMES : list.count;
    g_menuFrames = (Texture2D *)RL_CALLOC(max_to_load, sizeof(Texture2D));
    
    // Carrega frames até atingir MAX_MENU_FRAMES
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
    
    if (g_menuFrameCount > 0) {
        g_menuVideoLoaded = true;
        g_menuCurrentFrame = 0;
        g_menuFrameTimer = 0.0f;
        TraceLog(LOG_INFO, TextFormat("[MenuVideo] ✓ Carregados %d frames com sucesso", g_menuFrameCount));
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

#include "resources.h"
#include "raylib.h"
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

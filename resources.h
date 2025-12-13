#ifndef RESOURCES_H
#define RESOURCES_H

#include "screens.h"

// Módulo de recursos (assets): centraliza carregamento e descarregamento
// para evitar duplicação e facilitar o gerenciamento de memória.
// Carrega a logo do jogo (se existir em caminhos conhecidos).
void Resources_LoadLogo(LogoData *logo);

// Descarrega a logo (se foi carregada).
void Resources_UnloadLogo(LogoData *logo);

// Tileset medieval (opcional). Se presente, será usado para desenhar o mapa.
// O tileset esperado é `assets/medieval_tileset.png` com tiles quadrados.
// Se não existir, o render usará formas primitivas como fallback.
bool Resources_LoadTileset(const char *path);
void Resources_UnloadTileset(void);
bool Resources_HasTileset(void);
Texture2D Resources_GetTileset(void);

// Menu background video (looping)
// Path expected example: "assets/fundo_menu.webm"
bool Resources_LoadMenuVideo(const char *path);
void Resources_UnloadMenuVideo(void);
bool Resources_HasMenuVideo(void);
Texture2D Resources_GetMenuVideoTexture(void);
void Resources_UpdateMenuVideo(void);

// Background parallax para as fases
bool Resources_LoadBackground(const char *path);
void Resources_UnloadBackground(void);
bool Resources_HasBackground(void);
Texture2D Resources_GetBackground(void);

#endif // RESOURCES_H
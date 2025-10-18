// screens.h

#ifndef SCREENS_H
#define SCREENS_H

#include "raylib.h"
#include <stdbool.h>


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// 1. Tipos de Estado (GameScreen e CreditsState) - Crucial para a main
typedef enum GameScreen { MENU, GAME, CREDITS, EXIT } GameScreen;
typedef enum CreditsState { LOGO_FADE_IN, LOGO_HOLD, ROLLING_CREDITS } CreditsState;
typedef struct {
    Texture2D texture;
    bool loaded;
} LogoData;

// 2. Protótipos das funções que a main usará para desenhar/atualizar
// O protótipo deve retornar o próximo estado do jogo (GameScreen)
GameScreen UpdateCreditsScreen(GameScreen currentScreen);
void DrawCreditsScreen(float *scrollY, bool *returnToMenu, CreditsState *creditsState, float *logoAlpha, float *stateTimer, float *particleTimer, LogoData *logo);
void UpdateMenuScreen(GameScreen *currentScreen);
void DrawMenuScreen(GameScreen *currentScreen);

#endif // SCREENS_H
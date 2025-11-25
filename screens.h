#ifndef SCREENS_H
#define SCREENS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "raylib.h"
#include <stdbool.h>

/* Tamanho da janela */
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

/* Estados da aplicação */
typedef enum GameScreen {
    MENU,
    GAME,
    CREDITS,
    LEVEL_TRANSITION,
    EXIT
} GameScreen;

typedef enum CreditsState {
    LOGO_FADE_IN,
    LOGO_HOLD,
    ROLLING_CREDITS
} CreditsState;

typedef struct {
    Texture2D texture;
    bool loaded;
} LogoData;

/* Menu */
void UpdateMenuScreen(GameScreen *currentScreen);
void DrawMenuScreen(GameScreen *currentScreen);

/* Créditos: note que o segundo argumento é OUT-flag (bool *returnToMenu) */
void UpdateCreditsScreen(GameScreen *currentScreen);
void DrawCreditsScreen(float *scrollY, bool *returnToMenu, CreditsState *creditsState,
                       float *logoAlpha, float *stateTimer, float *particleTimer, LogoData *logo);

/* Game over */
void UpdateGameOver(GameScreen *currentScreen);
void DrawGameOver(GameScreen *currentScreen);

#ifdef __cplusplus
}
#endif

#endif // SCREENS_H

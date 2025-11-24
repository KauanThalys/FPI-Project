#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "game.h" // Para ter acesso a EstadoJogo
#include "screens.h"
#include "resources.h"

// Funções de Desenho
void Graphics_DrawMenu(GameScreen *currentScreen);
void Graphics_DrawCredits(float *scrollY, bool *returnToMenu, CreditsState *creditsState, float *logoAlpha, float *stateTimer, float *particleTimer, LogoData *logo);
void Graphics_DrawStageIntro(int level, float timer);
void Graphics_DrawGame(EstadoJogo *state);

// --- NOVA FUNÇÃO QUE ESTAVA FALTANDO ---
void Graphics_DrawLevelComplete(void);

#endif // GRAPHICS_H
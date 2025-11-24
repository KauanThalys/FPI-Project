#include <stdio.h>
#include "raylib.h"
#include <stdlib.h>
#include <stdbool.h>

//headers 
#include "screens.h" 
#include "resources.h"
#include "input.h"
#include "graphics.h"
#include "game.h"

//  função de desenho 
extern void Graphics_DrawLevelComplete(void);

int main() {
    // ----------------------------------------------------------------------------------
    // Inicialização
    // ----------------------------------------------------------------------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tales of Cineria");
    SetTargetFPS(60);
    InitAudioDevice(); 

    // status Inicial
    GameScreen telaAtual = MENU;
    GameScreen telaAnterior = MENU; 
    
    // Créditos
    CreditsState estadoCreditos = LOGO_FADE_IN;
    float scrollCreditos = SCREEN_HEIGHT;
    float alphaLogo = 0.0f;
    float timerEstado = 0.0f;
    float timerParticulas = 0.0f;

    // Variáveis da Intro da Fase
    float timerIntro = 0.0f;
    const float duracaoIntro = 3.0f; 
    int faseIntro = 1;

    // Variável para Transição de Nível 
    float timerTransicao = 0.0f;

    LogoData logo = {0};
    Resources_LoadLogo(&logo);
    Resources_LoadTileset("assets/medieval_tileset.png");

    // Inicializa o jogo 
    Jogo_Iniciar();

    // ----------------------------------------------------------------------------------
    // Loop Principal
    // ----------------------------------------------------------------------------------
    while (!WindowShouldClose()) {
        bool returnToMenu = false;

        switch (currentScreen) {
            case MENU:
                UpdateMenuScreen(&currentScreen); // Executa a lógica do mouse/input
                if (currentScreen == CREDITS) {
                    creditsState = LOGO_FADE_IN;
                    creditsScrollY = SCREEN_HEIGHT;
                    logoAlpha = 0.0f;
                    stateTimer = 0.0f;
                }
                break;

            case GAME:
                currentScreen = MENU;
                // Lógica do jogo (a ser implementada)
                break;

            case CREDITS:
                if (IsKeyPressed(KEY_ESCAPE)) {
                    returnToMenu = true;
                }
                break;

            case EXIT:
                CloseWindow();
                return 0;
        }

        BeginDrawing();
            
            ClearBackground(BLACK); 

        if (currentScreen == MENU) {
            DrawMenuScreen(&currentScreen);
        } else if (currentScreen == CREDITS) {
            DrawCreditsScreen(&creditsScrollY, &returnToMenu, &creditsState, &logoAlpha, &stateTimer, &particleTimer, &logo);
            
            if (returnToMenu) {
                currentScreen = MENU;
            }
        }

        EndDrawing();
    }

    if (logo.loaded) {
        UnloadTexture(logo.texture);
    }

    CloseWindow();

    return 0;
}
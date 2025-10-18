#include <stdio.h>
#include "raylib.h"
#include <stdlib.h>
#include <stdbool.h>

#include "screens.h" // Inclua o contrato do novo módulo


int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tales of Cineria");
    SetTargetFPS(60);

    GameScreen currentScreen = MENU;
    
    CreditsState creditsState = LOGO_FADE_IN;
    float creditsScrollY = SCREEN_HEIGHT;
    float logoAlpha = 0.0f;
    float stateTimer = 0.0f;
    float particleTimer = 0.0f;

    LogoData logo = {0};
    logo.loaded = false;
    const char* logoPaths[] = {"logo.png", "logo.jpg", "assets/logo.png"};
    for (int i = 0; i < 3; i++) {
        if (FileExists(logoPaths[i])) {
            logo.texture = LoadTexture(logoPaths[i]);
            logo.loaded = true;
            break;
        }
    }

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
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

    float gameTimer = 0.0f;
    bool gameRunning = false;

    Texture2D gamePlaceholderTexture = LoadTexture("assets/max.png");
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
        switch (currentScreen) {
            case MENU:
                UpdateMenuScreen(&currentScreen); // Executa a lógica do mouse/input
                if (currentScreen == CREDITS) {
                    creditsState = LOGO_FADE_IN;
                    creditsScrollY = SCREEN_HEIGHT;
                    logoAlpha = 0.0f;
                    stateTimer = 0.0f;
                }
                if(currentScreen == GAME){
                    gameTimer = 0.0f;
                    gameRunning = true;
                }                
                break;
            case GAME:
                if (gameRunning) {
                    gameTimer += GetFrameTime(); // <--- AGORA O TIMER ATUALIZA!
                    // CONDIÇÃO DE GAME OVER (A cada 5 segundos para teste)
                    if (gameTimer >= 5.0f) {
                        currentScreen = GAME_OVER;
                        gameRunning = false; // Pausa a lógica
                    }

                    // Teste manual de Game Over (Opcional, mas útil)
                    if (IsKeyPressed(KEY_G)) {
                        currentScreen = GAME_OVER;
                        gameRunning = false;
                    }
                }
                break;
            case CREDITS:
                UpdateCreditsScreen(&currentScreen); 
                break;
            case EXIT:
                CloseWindow();
                return 0;
            case GAME_OVER:
                UpdateGameOver(&currentScreen);
                if(currentScreen == GAME){
                    gameTimer = 0.0f;
                    gameRunning = true;
                }
                else if(currentScreen == MENU) gameRunning = false;
                break;
        }

        BeginDrawing();

        ClearBackground(BLACK); 

        if (currentScreen == GAME || currentScreen == GAME_OVER) {
            // Agora desenha a imagem placeholder
            float scale = 0.7f;
            int x = SCREEN_WIDTH/2 - (gamePlaceholderTexture.width * scale)/2;
            int y = SCREEN_HEIGHT/2 - (gamePlaceholderTexture.height * scale)/2;
            DrawTextureEx(gamePlaceholderTexture, (Vector2){(float)x, (float)y}, 0.0f, scale, WHITE);

            // Desenha o timer por cima da imagem (para mostrar que o jogo estava rodando)
            DrawText(TextFormat("Tempo: %.1f", gameTimer), 10, 10, 30, YELLOW);
        } 
        switch(currentScreen){
            case MENU:
                DrawMenuScreen(&currentScreen);
                break;
            case CREDITS:
                DrawCreditsScreen(&creditsScrollY, &currentScreen, &creditsState, &logoAlpha, &stateTimer, &particleTimer, &logo);
                break;
            case GAME_OVER:
                DrawGameOver(&currentScreen);
                break;
            default:
                break;
        }

        EndDrawing();
    }

    if (logo.loaded) {
        UnloadTexture(logo.texture);
    }

    UnloadTexture(gamePlaceholderTexture);
    CloseWindow();
    return 0;
}
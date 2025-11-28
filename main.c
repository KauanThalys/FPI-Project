// main.c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "raylib.h"
#include "screens.h"
#include "resources.h"
#include "input.h"
#include "graphics.h"
#include "game.h"
#include "audio.h"

int main(void) {
    // --- Inicialização ---
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tales of Cineria");
    SetTargetFPS(60);

    InitAudioDevice();
    Audio_Init();
    Audio_SetMasterVolume(0.9f); // ajuste opcional de volume

    GameScreen currentScreen = MENU;

    // --- Créditos ---
    CreditsState creditsState = LOGO_FADE_IN;
    float creditsScrollY = (float)SCREEN_HEIGHT;
    float logoAlpha = 0.0f;
    float stateTimer = 0.0f;
    float particleTimer = 0.0f;
    bool returnToMenu = false;

    // --- Intro / transição ---
    float timerIntro = 0.0f;
    const float duracaoIntro = 3.0f;
    int faseIntro = 1;
    bool showStageIntro = false;

    float timerTransicao = 0.0f;
    const float duracaoTransicao = 2.0f;

    // --- Recursos ---
    LogoData logo = { 0 };
    Resources_LoadLogo(&logo);
    Resources_LoadTileset("assets/medieval_tileset.png");
    // Carrega os frames do menu (extraídos com ffmpeg)
    Resources_LoadMenuVideo("assets/frames");

    Jogo_Iniciar();

    // --- Loop principal ---
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // --- Atualização ---
        Input_Update();
        Audio_Update(currentScreen, dt); // atualização do áudio

        // Lógica por tela
        switch (currentScreen) {
            case MENU:
                Input_HandleMenu(&currentScreen);
                if (currentScreen == CREDITS) {
                    creditsState = LOGO_FADE_IN;
                    creditsScrollY = (float)SCREEN_HEIGHT;
                    logoAlpha = 0.0f;
                    stateTimer = 0.0f;
                    particleTimer = 0.0f;
                }
                break;

            case GAME:
                Jogo_Atualizar(&currentScreen);
                break;

            case LEVEL_TRANSITION:
                timerTransicao += dt;
                if (timerTransicao >= duracaoTransicao) {
                    EstadoJogo *s = Jogo_ObterEstado();
                    Jogo_IniciarFase(s->level);
                    timerTransicao = 0.0f;
                    currentScreen = GAME;
                }
                break;
            case GAME_OVER:
                    // chama a função de update do painel de Game Over (botões)
                    UpdateGameOver(&currentScreen);
                    // permitir voltar com ESC diretamente (opcional)
                     if (Input_IsEscapePressed()) currentScreen = MENU;
                break;


            case CREDITS:
                if (Input_IsEscapePressed()) {
                    currentScreen = MENU;
                }
                break;

            case EXIT:
                // Sair do jogo
                CloseWindow();
                return 0;
        }

        // --- Renderização ---
        BeginDrawing();
        ClearBackground(BLACK);

        if (currentScreen == MENU) {
            DrawMenuScreen(&currentScreen);
        }
        else if (currentScreen == CREDITS) {
            returnToMenu = false;
            DrawCreditsScreen(&creditsScrollY, &returnToMenu, &creditsState,
                              &logoAlpha, &stateTimer, &particleTimer, &logo);
            if (returnToMenu) currentScreen = MENU;
        }
        else if (currentScreen == GAME) {
            if (showStageIntro) {
                timerIntro += dt;
                Graphics_DrawStageIntro(faseIntro, timerIntro);
                if (timerIntro >= duracaoIntro) {
                    showStageIntro = false;
                    timerIntro = 0.0f;
                }
            } else {
                Graphics_DrawGame(NULL); // TODO: passar estado real do jogo se necessário
            }
        }
        else if (currentScreen == LEVEL_TRANSITION) {
            Graphics_DrawLevelComplete();
        }

        EndDrawing();
    }

    // --- Limpeza ---
    if (logo.loaded) UnloadTexture(logo.texture);
    Resources_UnloadMenuVideo();
    Audio_Unload();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

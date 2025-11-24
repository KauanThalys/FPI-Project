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

    // Recursos
    LogoData logo = {0};
    Resources_LoadLogo(&logo);
    Resources_LoadTileset("assets/medieval_tileset.png");

    // Inicializa o jogo 
    Jogo_Iniciar();

    // ----------------------------------------------------------------------------------
    // Loop Principal
    // ----------------------------------------------------------------------------------
    while (!WindowShouldClose()) {
        
        bool voltarAoMenuDeCreditos = false;
        GameScreen estadoInicioFrame = telaAtual;

        switch (telaAtual) {
            case MENU:
                Input_HandleMenu(&telaAtual);

                // Iniciar Jogo do Zero
                if (telaAtual == GAME && estadoInicioFrame == MENU) {
                    telaAtual = STAGE_INTRO;
                    timerIntro = 0.0f;
                    faseIntro = 1; 
                    Jogo_IniciarFase(1); // Reseta tudo para fase 1
                }
                
                // Ir para Créditos
                if (telaAtual == CREDITS && estadoInicioFrame == MENU) {
                    estadoCreditos = LOGO_FADE_IN;
                    scrollCreditos = SCREEN_HEIGHT;
                    alphaLogo = 0.0f;
                    timerEstado = 0.0f;
                }
                break;

            case STAGE_INTRO:
                timerIntro += GetFrameTime();
                // Permite pular a intro com Espaço ou Clique
                if ((timerIntro > 0.5f && (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) 
                    || timerIntro >= duracaoIntro) {
                    
                    telaAtual = GAME;
                }
                break;

            case GAME:
                // O Jogo_Atualizar verifica a porta e pode mudar telaAtual para LEVEL_TRANSITION
                Jogo_Atualizar(&telaAtual); 
                break;

            // --- LÓGICA DE PASSAGEM DE FASE ---
            case LEVEL_TRANSITION:
                timerTransicao += GetFrameTime();
                
                // Espera 3 segundos mostrando "FASE CONCLUÍDA"
                if (timerTransicao > 3.0f) {
                    timerTransicao = 0.0f;
                    
                    // Pega o nível que foi incrementado no game.c 
                    int proximoNivel = Jogo_ObterEstado()->level;
                    
                    // Carrega o mapa da próxima fase e reseta o player
                    Jogo_IniciarFase(proximoNivel);
                    
                    // Prepara a Intro para mostrar o título da nova fase
                    faseIntro = proximoNivel;
                    timerIntro = 0.0f;
                    
                    // Volta para o fluxo de Intro -> Game
                    telaAtual = STAGE_INTRO; 
                }
                break;

            case CREDITS:
                if (Input_IsEscapePressed()) {
                    voltarAoMenuDeCreditos = true;
                }
                break;

            case EXIT:
                break;
        }

        if (telaAtual == EXIT) break;

        if (voltarAoMenuDeCreditos) {
            telaAtual = MENU;
        }

        // ------------------------------------------------------------------------------
        // GERENCIAMENTO DE JANELA (Fullscreen sem bordas)
        // ------------------------------------------------------------------------------
        if (telaAtual != estadoInicioFrame) {
            bool modoJogo = (telaAtual == STAGE_INTRO || telaAtual == GAME || telaAtual == LEVEL_TRANSITION);
            bool estavaMenu = (estadoInicioFrame == MENU || estadoInicioFrame == CREDITS);

            // Entrando no jogo -> Tela Cheia
            if (modoJogo && estavaMenu) {
                int monitor = GetCurrentMonitor();
                int mw = GetMonitorWidth(monitor);
                int mh = GetMonitorHeight(monitor);
                SetWindowState(FLAG_WINDOW_UNDECORATED);
                SetWindowSize(mw, mh);
                SetWindowPosition(0, 0);
            }
            // Voltando pro menu -> Janela Normal
            else if (telaAtual == MENU) {
                if (IsWindowState(FLAG_WINDOW_UNDECORATED)) {
                    ClearWindowState(FLAG_WINDOW_UNDECORATED);
                    int monitor = GetCurrentMonitor();
                    int mw = GetMonitorWidth(monitor);
                    int mh = GetMonitorHeight(monitor);
                    SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
                    SetWindowPosition((mw - SCREEN_WIDTH) / 2, (mh - SCREEN_HEIGHT) / 2);
                }
            }
        }

        telaAnterior = telaAtual;

        // ------------------------------------------------------------------------------
        // DRAW (Desenho)
        // ------------------------------------------------------------------------------
        BeginDrawing();
            
            ClearBackground(BLACK); 

            if (telaAtual == MENU) {
                Graphics_DrawMenu(&telaAtual);
            } 
            else if (telaAtual == STAGE_INTRO) {
                Graphics_DrawStageIntro(faseIntro, timerIntro);
            } 
            else if (telaAtual == GAME) {
                Graphics_DrawGame(Jogo_ObterEstado());
            } 
            else if (telaAtual == LEVEL_TRANSITION) {
                // Desenha a tela de sucesso 
                Graphics_DrawLevelComplete();
            }
            else if (telaAtual == CREDITS) {
                Graphics_DrawCredits(&scrollCreditos, &voltarAoMenuDeCreditos, &estadoCreditos, &alphaLogo, &timerEstado, &timerParticulas, &logo);
            }

        EndDrawing();
    }

    // ----------------------------------------------------------------------------------
    // Finalização
    // ----------------------------------------------------------------------------------
    Resources_UnloadLogo(&logo);
    Resources_UnloadTileset();
    Jogo_Descarregar();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
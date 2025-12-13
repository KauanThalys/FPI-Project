// screens.c
#include <stdio.h>
#include "raylib.h"
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "screens.h"
#include "resources.h"

void DrawSolidBlackBackground(void) {
    ClearBackground(BLACK); 
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, 150, Fade(BLACK, 0.9f), Fade(BLACK, 0.0f));
    DrawRectangleGradientV(0, SCREEN_HEIGHT - 150, SCREEN_WIDTH, 150, Fade(BLACK, 0.0f), Fade(BLACK, 0.9f));
}

//---------------- CREDITS SCREEN FUNCTIONS ----------------//
void UpdateCreditsScreen(GameScreen *currentScreen) {
    Rectangle btnBack = {20, SCREEN_HEIGHT - 60, 140, 40};
    Color btnColor = CheckCollisionPointRec(GetMousePosition(), btnBack) ? DARKGRAY : GRAY;
    if (CheckCollisionPointRec(GetMousePosition(), btnBack) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        *currentScreen = MENU;
    }
}

/*
 * DrawCreditsScreen:
 *  - scrollY: pointer in/out controlling vertical position
 *  - returnToMenu: out flag; when set to true caller must switch screen to MENU
 *  - creditsState, logoAlpha, stateTimer, particleTimer, logo: refs for animation/resources
 */
void DrawCreditsScreen(float *scrollY, bool *returnToMenu, CreditsState *creditsState, float *logoAlpha, float *stateTimer, float *particleTimer, LogoData *logo) {
    DrawSolidBlackBackground();

    // Atualiza timers usados na animação dos créditos
    *particleTimer += GetFrameTime(); 
    *stateTimer += GetFrameTime();
    switch (*creditsState) {
        case LOGO_FADE_IN:
            // Fade in da logo
            *logoAlpha += GetFrameTime() * 0.8f;
            if (*logoAlpha >= 1.0f) {
                *logoAlpha = 1.0f;
                *creditsState = LOGO_HOLD; 
                *stateTimer = 0.0f;
            }
            break;
        case LOGO_HOLD:
            // Mantém a logo visível por um intervalo antes da rolagem
            if (*stateTimer >= 2.5f) { 
                *creditsState = ROLLING_CREDITS;
                *stateTimer = 0.0f;
                
                float logoHeight = 0;
                if (logo->loaded) {
                    float maxWidth = 500.0f, maxHeight = 300.0f;
                    float scale = fminf(maxWidth / logo->texture.width, maxHeight / logo->texture.height);
                    logoHeight = logo->texture.height * scale;
                } else {
                    logoHeight = 150;
                }
                *scrollY = SCREEN_HEIGHT/2 - logoHeight/2; 
            }
            break;
        case ROLLING_CREDITS:
            break;
    }

    // Desenha logo/fallback durante fade/hold
    if (*creditsState == LOGO_FADE_IN || *creditsState == LOGO_HOLD) {
        if (*logoAlpha > 0.0f) {
            if (logo->loaded) {
                float maxWidth = 500.0f, maxHeight = 300.0f;
                float scale = fminf(maxWidth / logo->texture.width, maxHeight / logo->texture.height);
                float logoWidth = logo->texture.width * scale;
                float logoHeight = logo->texture.height * scale;
                
                DrawTextureEx(logo->texture, (Vector2){SCREEN_WIDTH/2 - logoWidth/2, SCREEN_HEIGHT/2 - logoHeight/2}, 0.0f, scale, Fade(WHITE, *logoAlpha));
            } else {
                const char *title = "TALES OF";
                const char *subtitle = "CINERIA";
                int titleSize = 60, subtitleSize = 80;
                int titleWidth = MeasureText(title, titleSize);
                int subtitleWidth = MeasureText(subtitle, subtitleSize);
                int centerY = SCREEN_HEIGHT / 2 - 80;

                DrawText(title, SCREEN_WIDTH/2 - titleWidth/2, centerY, titleSize, Fade((Color){160, 82, 45, 255}, *logoAlpha));
                
                float emberPulse = 0.8f + sinf(*stateTimer * 4.0f) * 0.2f;
                DrawText(subtitle, SCREEN_WIDTH/2 - subtitleWidth/2, centerY + 70, subtitleSize, Fade((Color){220, 20, 60, 255}, *logoAlpha * emberPulse));
            }
        }
    }

    // ROLAGEM DOS CRÉDITOS
    if (*creditsState == ROLLING_CREDITS) {
        *scrollY -= 0.8f; // velocidade da rolagem
        int currentY = (int)*scrollY;
    
        float logoHeight = 0;
        if (logo->loaded) {
            float maxWidth = 500.0f, maxHeight = 300.0f;
            float scale = fminf(maxWidth / logo->texture.width, maxHeight / logo->texture.height);
            float logoWidth = logo->texture.width * scale;
            logoHeight = logo->texture.height * scale;
            
            DrawTextureEx(logo->texture, (Vector2){SCREEN_WIDTH/2 - logoWidth/2, (float)currentY}, 0.0f, scale, WHITE);
        } else {
            const char *title = "TALES OF";
            const char *subtitle = "CINERIA";
            logoHeight = 150;
            DrawText(title, SCREEN_WIDTH/2 - MeasureText(title, 60)/2, currentY, 60, (Color){160, 82, 45, 255});
            DrawText(subtitle, SCREEN_WIDTH/2 - MeasureText(subtitle, 80)/2, currentY + 70, 80, (Color){220, 20, 60, 255});
        }
        
        currentY += (int)logoHeight + 100;
        
        const char *creditSections[][20] = {
            {"INTEGRANTES DO GRUPO", "END"},
            {"LUCAS RYAN<lrsl2>", "Suporte \nTela inicial \nSprites", "END"},
            {"ÁLVARO MIGUEL", "Sons \nSprites \nSuporte", "END"},
            {"PEDRO ALBUQUERQUE<amol>", "Coletável \nMapa \nPassagem de nível", "END"},
            {"JOÃO GABRIEL<jgada>", "Mapas \nLogo \nCréditos", "END"},
            {"Obrigado por jogar!", "TALES OF CINERIA © 2025", "END"}
        };
        
        int numSections = 6;
        
        // Loop de desenho das seções
        for (int s = 0; s < numSections; s++) {
            bool alignRight = (s % 2 == 0); // Alinhamento da Seção
            
            for (int i = 0; strcmp(creditSections[s][i], "END") != 0; i++) {
                
                int fontSize = 20;
                Color textColor = WHITE;
                int posX = 0;
                
                // TÍTULO DA SEÇÃO
                if (i == 0 && s < numSections - 1) { 
                    fontSize = 30;
                    textColor = (Color){207, 181, 59, 255}; // Ouro
                    posX = alignRight ? SCREEN_WIDTH - 100 - MeasureText(creditSections[s][i], fontSize) : 100;
                } 
                else if (s == 0) {
                    // INTEGRANTES estilo uniforme
                    fontSize = 20;
                    textColor = (Color){255, 140, 0, 255};
                    posX = alignRight ? SCREEN_WIDTH - 100 - MeasureText(creditSections[s][i], fontSize) : 100;
                }
                // ITENS DE CONTEÚDO
                else {    
                    if (s == 4) { // FERRAMENTAS
                        if (i % 2 != 0) { // Rótulo
                            fontSize = 22;
                            textColor = (Color){255, 140, 0, 255};
                        } else { // Valor
                            fontSize = 18;
                            textColor = (Color){122, 122, 122, 255};
                        }
                    } else {
                        if (i % 2 != 0) {
                            fontSize = 18;
                            textColor = (Color){122, 122, 122, 255}; 
                        } else {
                            fontSize = 22;
                            textColor = (Color){255, 140, 0, 255}; 
                        }
                    }

                    posX = alignRight ? SCREEN_WIDTH - 100 - MeasureText(creditSections[s][i], fontSize) : 100;
                }
                
                // Mensagem final centralizada
                if (s == numSections - 1) { 
                    posX = SCREEN_WIDTH/2 - MeasureText(creditSections[s][i], fontSize)/2;
                    textColor = (i == 0) ? (Color){220, 20, 60, 255} : WHITE;
                }

                // Fade quando entra/sai da tela
                float alpha = 1.0f;
                if (currentY < 150) alpha = (currentY - 50) / 100.0f;
                else if (currentY > SCREEN_HEIGHT - 150) alpha = (SCREEN_HEIGHT - 50 - currentY) / 100.0f;
                
                if (currentY > 50 && currentY < SCREEN_HEIGHT - 50) {
                    DrawText(creditSections[s][i], posX + 2, currentY + 2, fontSize, Fade(BLACK, alpha * 0.7f));
                    DrawText(creditSections[s][i], posX, currentY, fontSize, Fade(textColor, alpha));
                }
                
                // Espaçamento vertical
                if (s == 0) { 
                    currentY += 35;
                } else {
                    currentY += (i % 2 == 0 && i > 0) ? 50 : 35;
                }
            }
            currentY += 100;
        }
        
        // reinicia os creditos qnd terminar
        // condição segura para reiniciar após toda a área ter rolado pra cima
        if (*scrollY < - (currentY - (int)*scrollY + 200)) {
            *creditsState = LOGO_FADE_IN;
            *scrollY = SCREEN_HEIGHT;
            *logoAlpha = 0.0f;
            *stateTimer = 0.0f;
        }
    }

    // botao de voltar (sinaliza via returnToMenu)
    Rectangle btnBack = {20, SCREEN_HEIGHT - 60, 140, 40};
    Color btnColor = CheckCollisionPointRec(GetMousePosition(), btnBack) ? DARKGRAY : GRAY;
    
    DrawRectangleRec(btnBack, btnColor);
    DrawRectangleLinesEx(btnBack, 2, LIGHTGRAY);
    DrawText("VOLTAR", btnBack.x + 35, btnBack.y + 12, 18, WHITE);
    DrawText("ESC", SCREEN_WIDTH - 50, SCREEN_HEIGHT - 30, 15, DARKGRAY);

    // detecta clique e marca flag de retorno
    if (CheckCollisionPointRec(GetMousePosition(), btnBack) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (returnToMenu) *returnToMenu = true;
    }
}

//---------------- MENU SCREEN FUNCTIONS ----------------//
void UpdateMenuScreen(GameScreen *currentScreen) {
    Rectangle btnCredits = {SCREEN_WIDTH/2 - 125, 300, 250, 50};
    Rectangle btnGame = {SCREEN_WIDTH/2 - 125, 220, 250, 50};
    Rectangle btnExit = {SCREEN_WIDTH/2 - 125, 380, 250, 50};
    if (CheckCollisionPointRec(GetMousePosition(), btnCredits) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        *currentScreen = CREDITS;
    }
    if (CheckCollisionPointRec(GetMousePosition(), btnGame) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        *currentScreen = GAME;
    }
    if (CheckCollisionPointRec(GetMousePosition(), btnExit) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        *currentScreen = EXIT;
    }
}

void DrawMenuScreen(GameScreen *currentScreen) {
    // Atualiza e desenha vídeo de fundo se houver
    if (Resources_HasMenuVideo()) {
        Resources_UpdateMenuVideo();
        Texture2D vidTex = Resources_GetMenuVideoTexture();
        if (vidTex.width > 0) {
            // Ajusta para preencher a tela mantendo proporção
            float scaleX = (float)SCREEN_WIDTH / (float)vidTex.width;
            float scaleY = (float)SCREEN_HEIGHT / (float)vidTex.height;
            float scale = fmaxf(scaleX, scaleY); // cover
            float drawW = vidTex.width * scale;
            float drawH = vidTex.height * scale;
            DrawTextureEx(vidTex, (Vector2){SCREEN_WIDTH/2 - drawW/2, SCREEN_HEIGHT/2 - drawH/2}, 0.0f, scale, WHITE);
        } else {
            ClearBackground((Color){10, 10, 20, 255});
        }
    } else {
        ClearBackground((Color){10, 10, 20, 255});
    }
    DrawText("TALES OF CINERIA", SCREEN_WIDTH/2 - MeasureText("TALES OF CINERIA", 50)/2, 100, 50, GOLD);
    
    Rectangle btnGame = {SCREEN_WIDTH/2 - 125, 220, 250, 50};
    Rectangle btnCredits = {SCREEN_WIDTH/2 - 125, 300, 250, 50};  
    Rectangle btnExit = {SCREEN_WIDTH/2 - 125, 380, 250, 50}; 

    bool hoverGame = CheckCollisionPointRec(GetMousePosition(), btnGame); 
    bool hoverCredits = CheckCollisionPointRec(GetMousePosition(), btnCredits); 
    bool hoverExit = CheckCollisionPointRec(GetMousePosition(), btnExit); 

    DrawRectangleRec(btnGame, hoverGame ? DARKBLUE : BLUE);
    DrawText("JOGAR", btnGame.x + 65, btnGame.y + 15, 20, WHITE);

    DrawRectangleRec(btnCredits, hoverCredits ? DARKBLUE : BLUE);
    DrawText("CRÉDITOS", btnCredits.x + 65, btnCredits.y + 15, 20, WHITE);

    DrawRectangleRec(btnExit, hoverExit ? DARKBLUE : BLUE);
    DrawText("SAIR", btnExit.x + 65, btnExit.y + 15, 20, WHITE);
}

//---------------- GAME OVER SCREEN FUNCTIONS ----------------//
void UpdateGameOver(GameScreen *currentScreen) {
    Rectangle btnReset = {SCREEN_WIDTH/2 - 125, 220, 250, 50};
    Rectangle btnMenu = {SCREEN_WIDTH/2 - 125, 300, 250, 50}; 

    if (CheckCollisionPointRec(GetMousePosition(), btnReset) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        *currentScreen = GAME; //MUDAR PARA GAME QUANDO TIVER O JOGO
    }
    if (CheckCollisionPointRec(GetMousePosition(), btnMenu) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        *currentScreen = MENU;
    }
}

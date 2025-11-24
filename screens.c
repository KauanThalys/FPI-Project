#include <stdio.h>
// Arquivo de telas: menu, créditos e helpers de desenho.
#include "raylib.h"
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "screens.h"

void DrawSolidBlackBackground() {
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

void DrawCreditsScreen(float *scrollY, GameScreen *currentScreen, CreditsState *creditsState, float *logoAlpha, float *stateTimer, float *particleTimer, LogoData *logo) {
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

    if (*creditsState == LOGO_FADE_IN || *creditsState == LOGO_HOLD) {
        if (*logoAlpha > 0.0f) {
            if (logo->loaded) {
                float maxWidth = 500.0f, maxHeight = 300.0f;
                float scale = fminf(maxWidth / logo->texture.width, maxHeight / logo->texture.height);
                float logoWidth = logo->texture.width * scale;
                float logoHeight = logo->texture.height * scale;
                
                DrawTextureEx(logo->texture, (Vector2){SCREEN_WIDTH/2 - logoWidth/2, SCREEN_HEIGHT/2 - logoHeight/2}, 0.0f, scale, Fade(WHITE, *logoAlpha));
            } else {
                // fallback da logo
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

    if (*creditsState == ROLLING_CREDITS) {
        *scrollY -= 0.8f; // velocidade da rolagem
        int currentY = *scrollY;
    
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
        
        currentY += logoHeight + 100;
        
        const char *creditSections[][20] = {
            // SEÇÃO 0: INTEGRANTES DO GRUPO (Formato Nome/Nome/Nome... - Uma Coluna)
            {"INTEGRANTES DO GRUPO", "Alvaro Lima <amol>", "Joao Drummond <jgada>", "Pedro Albuquerque <phma2>", "Kauan Thalys <ktn>", "END"},
            
            // SEÇÃO 1: DESIGN E ARTE
            {"DESIGN E ARTE", "Artista Conceitual", "Nome Artista", "UI/UX Designer", "Nome Designer", "END"},
            
            // SEÇÃO 2: ÁUDIO
            {"ÁUDIO", "Compositor", "Nome Compositor", "Efeitos Sonoros", "Nome Sound Designer", "END"},
            
            // SEÇÃO 3: AGRADECIMENTOS ESPECIAIS
            {"AGRADECIMENTOS ESPECIAIS", "Professor", "Alexandre Cabral Mota", "Universidade", "Universidade Federal de Pernambuco", "END"},
            
            // SEÇÃO 4: FERRAMENTAS UTILIZADAS (Rótulo/Valor Adjacentes)
            {"FERRAMENTAS UTILIZADAS", "Engine Gráfica", "Raylib 5.0", "Linguagem", "C (C99)", "END"},
            
            // SEÇÃO 5: MENSAGEM FINAL (Obrigado por jogar!)
            {"Obrigado por jogar!", "TALES OF CINERIA © 2025", "END"}
        };
        
        int numSections = 6;
        
        // --- 2. LOOP PRINCIPAL DE DESENHO (Lógica Central) ---
        for (int s = 0; s < numSections; s++) {
            bool alignRight = (s % 2 == 0); // Alinhamento da Seção (o título ímpar alinha à esquerda)
            
            for (int i = 0; strcmp(creditSections[s][i], "END") != 0; i++) { // Loop corrigido para buscar "END"
                
                // Ignora o primeiro item se for string vazia (não deve mais acontecer)
                // if (creditSections[s][i][0] == '\0') { currentY += 20; continue; } 

                int fontSize = 20;
                Color textColor = WHITE;
                int posX = 0;
                
                // --- DEFINIÇÃO DE ESTILO E POSIÇÃO ---
                
                // 1. Estilo TÍTULO da Seção (i=0, exceto a última)
                if (i == 0 && s < numSections - 1) { 
                    fontSize = 30;
                    textColor = (Color){207, 181, 59, 255}; // Ouro
                    posX = alignRight ? SCREEN_WIDTH - 100 - MeasureText(creditSections[s][i], fontSize) : 100;
                } 
                else if (s == 0) {
                    if (i > 0) { // Se não for o Título, use o estilo uniforme
                    fontSize = 20; // Uniforme
                    textColor = (Color){255, 140, 0, 255}; // Uniforme
                    posX = alignRight ? SCREEN_WIDTH - 100 - MeasureText(creditSections[s][i], fontSize) : 100;
                    }
                }
                // 2. Estilo ITENS DE CONTEÚDO (i > 0)
                else {    
                    // LÓGICA DE SOBREPOSIÇÃO PARA ESTILOS ESPECÍFICOS (FERRAMENTAS / PADRÃO)
                    if (s == 4) { // Seção FERRAMENTAS UTILIZADAS
                        if (i % 2 != 0) { // RÓTULO (Engine Gráfica, Linguagem)
                            fontSize = 22;
                            textColor = (Color){255, 140, 0, 255}; // Laranja
                        } else { // VALOR (Raylib 5.0, C(C99))
                            fontSize = 18;
                            textColor = (Color){122, 122, 122, 255}; // Cinza
                        }
                    } else { // Lógica Padrão do Colega (e INTEGRANTES)
                        if (i % 2 != 0) { // Item ÍMPAR (Nome do Colega, Cor Cinza)
                            fontSize = 18;
                            textColor = (Color){122, 122, 122, 255}; 
                        } else { // Item PAR (Cargo/Rótulo, Cor Laranja)
                            fontSize = 22;
                            textColor = (Color){255, 140, 0, 255}; 
                        }
                    }

                    // Posição X (Para Itens, usa o estilo da Seção)
                    posX = alignRight ? SCREEN_WIDTH - 100 - MeasureText(creditSections[s][i], fontSize) : 100;
                }
                
                // 3. Estilo Mensagem Final (Sobrescreve tudo para centralizar)
                if (s == numSections - 1) { 
                    posX = SCREEN_WIDTH/2 - MeasureText(creditSections[s][i], fontSize)/2;
                    textColor = (i == 0) ? (Color){220, 20, 60, 255} : WHITE;
                }

                // [CÓDIGO DE FADE E DRAW (MANTIDO)]
                float alpha = 1.0f;
                if (currentY < 150) alpha = (currentY - 50) / 100.0f;
                else if (currentY > SCREEN_HEIGHT - 150) alpha = (SCREEN_HEIGHT - 50 - currentY) / 100.0f;
                
                if (currentY > 50 && currentY < SCREEN_HEIGHT - 50) {
                    DrawText(creditSections[s][i], posX + 2, currentY + 2, fontSize, Fade(BLACK, alpha * 0.7f));
                    DrawText(creditSections[s][i], posX, currentY, fontSize, Fade(textColor, alpha));
                }
                
                // --- 4. CÁLCULO DE ESPAÇAMENTO Y ---
                if (s == 0) { 
                    currentY += 35; // CORREÇÃO: Espaçamento de 35px para lista de nomes
                } else {
                    currentY += (i % 2 == 0 && i > 0) ? 50 : 35; // Lógica de duas colunas para o resto
                }
            }
            currentY += 100; // Espaço entre as seções
        }
        
        // reinicia os creditos qnd terminar
        if (*scrollY < -(currentY - (int)*scrollY + 200)) {
            *creditsState = LOGO_FADE_IN;
            *scrollY = SCREEN_HEIGHT;
            *logoAlpha = 0.0f;
            *stateTimer = 0.0f;
        }
    }

    // botao de voltar
    Rectangle btnBack = {20, SCREEN_HEIGHT - 60, 140, 40};
    Color btnColor = CheckCollisionPointRec(GetMousePosition(), btnBack) ? DARKGRAY : GRAY;
    
    DrawRectangleRec(btnBack, btnColor);
    DrawRectangleLinesEx(btnBack, 2, LIGHTGRAY);
    DrawText("VOLTAR", btnBack.x + 35, btnBack.y + 12, 18, WHITE);
    DrawText("ESC", SCREEN_WIDTH - 50, SCREEN_HEIGHT - 30, 15, DARKGRAY);
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
        // Ir para o jogo (a gestão de fullscreen é feita em main.c)
        *currentScreen = GAME;
    }
    if (CheckCollisionPointRec(GetMousePosition(), btnExit) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        *currentScreen = EXIT;
    }
}

void DrawMenuScreen(GameScreen *currentScreen) {
    ClearBackground((Color){10, 10, 20, 255});
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
void DrawGameOver(GameScreen *currentScreen){
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f));
    DrawText("GAME OVER", SCREEN_WIDTH/2 - MeasureText("GAME OVER", 50)/2, 100, 50, RED);
    Rectangle btnReset = {SCREEN_WIDTH/2 - 125, 220, 250, 50};
    Rectangle btnMenu = {SCREEN_WIDTH/2 - 125, 300, 250, 50};  

    bool hoverReset = CheckCollisionPointRec(GetMousePosition(), btnReset); 
    bool hoverMenu = CheckCollisionPointRec(GetMousePosition(), btnMenu); 

    DrawRectangleRec(btnReset, hoverReset ? DARKBLUE : BLUE);
    DrawText("REINICIAR", btnReset.x + 65, btnReset.y + 15, 20, WHITE);

    DrawRectangleRec(btnMenu, hoverMenu ? DARKBLUE : BLUE);
    DrawText("MENU", btnMenu.x + 65, btnMenu.y + 15, 20, WHITE);
}
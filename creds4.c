// gcc creds4.c -o creditosv4 -lraylib -lm  --> cd "pasta onde ta o arquivo" e usa esse comando ai(nao sei como é no windows)
#include "raylib.h"
#include <stdbool.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef enum GameScreen { MENU, CREDITS } GameScreen;
typedef enum CreditsState { LOGO_FADE_IN, LOGO_HOLD, ROLLING_CREDITS } CreditsState;
typedef struct {
    Texture2D texture;
    bool loaded;
} LogoData;
void DrawSolidBlackBackground() {
    ClearBackground(BLACK); 
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, 150, Fade(BLACK, 0.9f), Fade(BLACK, 0.0f));
    DrawRectangleGradientV(0, SCREEN_HEIGHT - 150, SCREEN_WIDTH, 150, Fade(BLACK, 0.0f), Fade(BLACK, 0.9f));
}
void DrawCreditsScreen(float *scrollY, bool *returnToMenu, CreditsState *creditsState, float *logoAlpha, float *stateTimer, float *particleTimer, LogoData *logo) {
    DrawSolidBlackBackground();

    *particleTimer += GetFrameTime(); 
    *stateTimer += GetFrameTime();
    switch (*creditsState) {
        case LOGO_FADE_IN:
            *logoAlpha += GetFrameTime() * 0.8f;
            if (*logoAlpha >= 1.0f) {
                *logoAlpha = 1.0f;
                *creditsState = LOGO_HOLD; 
                *stateTimer = 0.0f;
            }
            break;
        case LOGO_HOLD:
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
        
        //nomes nos creditos(alterar antes de entregar)
        const char *creditSections[][20] = {
            {"DESENVOLVIMENTO", "", "Programador Principal", "Seu Nome", "", "Gameplay Designer", "Nome Colega 1", "END"},
            {"DESIGN E ARTE", "", "Artista Conceitual", "Nome Artista", "", "UI/UX Designer", "Nome Designer", "END"},
            {"ÁUDIO", "", "Compositor", "Nome Compositor", "", "Efeitos Sonoros", "Nome Sound Designer", "END"},
            {"AGRADECIMENTOS ESPECIAIS", "", "Professor", "Nome do Professor", "", "Universidade", "Sua Universidade", "END"},
            {"FERRAMENTAS UTILIZADAS", "", "Engine Gráfica", "Raylib 5.0", "", "Linguagem", "C (C99)", "END"},
            {"", "", "Obrigado por jogar!", "TALES OF CINERIA © 2025", "END"}
        };
        
        int numSections = 6;
        
        for (int s = 0; s < numSections; s++) {
            bool alignRight = (s % 2 == 0);  
            
            for (int i = 0; creditSections[s][i][0] != 'E'; i++) {
                if (creditSections[s][i][0] == '\0') { currentY += 20; continue; }

                int fontSize = 20;
                Color textColor = WHITE;
                int posX = 0;
                
                if (i == 0 && s < numSections - 1) { 
                    fontSize = 30;
                    textColor = (Color){207, 181, 59, 255}; 
                    posX = alignRight ? SCREEN_WIDTH - 100 - MeasureText(creditSections[s][i], fontSize) : 100;
                } else if (i % 2 != 0) { 
                    fontSize = 18;
                    textColor = (Color){122, 122, 122, 255}; 
                    posX = alignRight ? SCREEN_WIDTH - 100 - MeasureText(creditSections[s][i], fontSize) : 100;
                } else { 
                    fontSize = 22;
                    textColor = (Color){255, 140, 0, 255}; 
                    posX = alignRight ? SCREEN_WIDTH - 100 - MeasureText(creditSections[s][i], fontSize) : 100;
                }
                
                if (s == numSections - 1) { 
                    posX = SCREEN_WIDTH/2 - MeasureText(creditSections[s][i], fontSize)/2;
                    textColor = (i == 0) ? (Color){220, 20, 60, 255} : WHITE;
                }

                float alpha = 1.0f;
                if (currentY < 150) alpha = (currentY - 50) / 100.0f;
                else if (currentY > SCREEN_HEIGHT - 150) alpha = (SCREEN_HEIGHT - 50 - currentY) / 100.0f;
                
                if (currentY > 50 && currentY < SCREEN_HEIGHT - 50) {
                    DrawText(creditSections[s][i], posX + 2, currentY + 2, fontSize, Fade(BLACK, alpha * 0.7f));
                    DrawText(creditSections[s][i], posX, currentY, fontSize, Fade(textColor, alpha));
                }
                
                currentY += (i % 2 == 0 && i > 0) ? 50 : 35; 
            }
            currentY += 100; 
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
    if (CheckCollisionPointRec(GetMousePosition(), btnBack) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        *returnToMenu = true;
    }
    
    DrawRectangleRec(btnBack, btnColor);
    DrawRectangleLinesEx(btnBack, 2, LIGHTGRAY);
    DrawText("VOLTAR", btnBack.x + 35, btnBack.y + 12, 18, WHITE);
    DrawText("ESC", SCREEN_WIDTH - 50, SCREEN_HEIGHT - 30, 15, DARKGRAY);
}

void DrawMenuScreen(GameScreen *currentScreen) {
    ClearBackground((Color){10, 10, 20, 255});
    DrawText("TALES OF CINERIA", SCREEN_WIDTH/2 - MeasureText("TALES OF CINERIA", 50)/2, 100, 50, GOLD);

    Rectangle btnCredits = {SCREEN_WIDTH/2 - 125, 300, 250, 50};
    bool hover = CheckCollisionPointRec(GetMousePosition(), btnCredits);
    
    DrawRectangleRec(btnCredits, hover ? DARKBLUE : BLUE);
    DrawText("CRÉDITOS", btnCredits.x + 65, btnCredits.y + 15, 20, WHITE);

    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        *currentScreen = CREDITS;
    }
}

int main(void) {
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
                break;
            case CREDITS:
                if (IsKeyPressed(KEY_ESCAPE)) {
                    returnToMenu = true;
                }
                break;
        }

        BeginDrawing();

        if (currentScreen == MENU) {
            DrawMenuScreen(&currentScreen);
            
            if (currentScreen == CREDITS) {
                 creditsState = LOGO_FADE_IN;
                 creditsScrollY = SCREEN_HEIGHT;
                 logoAlpha = 0.0f;
                 stateTimer = 0.0f;
            }
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


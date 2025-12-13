#include "graphics.h"
#include <stdio.h>
#include "raylib.h"
#include "game.h"
#include <math.h>

// Funções externas (do screens.c)
extern void DrawMenuScreen(GameScreen *currentScreen);
extern void DrawCreditsScreen(float *scrollY, bool *returnToMenu, CreditsState *creditsState, float *logoAlpha, float *stateTimer, float *particleTimer, LogoData *logo);

// --- Wrappers ---
void Graphics_DrawMenu(GameScreen *currentScreen) { 
    DrawMenuScreen(currentScreen); 
}

void Graphics_DrawCredits(float *scrollY, bool *returnToMenu, CreditsState *creditsState, float *logoAlpha, float *stateTimer, float *particleTimer, LogoData *logo) { 
    DrawCreditsScreen(scrollY, returnToMenu, creditsState, logoAlpha, stateTimer, particleTimer, logo); 
}

// --- Intro da Fase ---
void Graphics_DrawStageIntro(int level, float timer) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangle(0, 0, screenW, screenH, BLACK);

    char buf[128];
    const char *title = NULL; 
    // extern const char* Levels_GetTitle(int level);
    // title = Levels_GetTitle(level);
    
    if (title) sprintf(buf, "%s", title);
    else sprintf(buf, "Fase %d", level);

    float alpha = 1.0f;
    if (timer < 1.0f) alpha = timer; 
    if (timer > 2.0f) alpha = 1.0f - (timer - 2.0f);
    if (alpha < 0) alpha = 0;

    int fontSize = 60;
    int textW = MeasureText(buf, fontSize);
    DrawText(buf, screenW/2 - textW/2, screenH/2 - 50, fontSize, Fade(MAROON, alpha));

    const char *hint = "Prepare-se...";
    int hintW = MeasureText(hint, 20);
    DrawText(hint, screenW/2 - hintW/2, screenH/2 + 30, 20, Fade(GRAY, alpha));
}


void Graphics_DrawLevelComplete(void) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    
    // Fundo semi-transparente ou preto
    DrawRectangle(0, 0, screenW, screenH, BLACK);
    
    const char *text = "FASE CONCLUIDA!";
    int fontSize = 50;
    int textW = MeasureText(text, fontSize);
    
    // Texto Dourado
    float time = (float)GetTime();
    Color color = (sin(time * 5) > 0) ? GOLD : ORANGE;

    DrawText(text, screenW/2 - textW/2, screenH/2 - 50, fontSize, color);
    
    const char *sub = "Carregando proximo desafio...";
    int subW = MeasureText(sub, 20);
    DrawText(sub, screenW/2 - subW/2, screenH/2 + 20, 20, LIGHTGRAY);
}

// --- Desenho do Jogo  ---
void Graphics_DrawGame(EstadoJogo *state) {
    EstadoJogo *estadoLocal = state ? state : Jogo_ObterEstado();

    ClearBackground((Color){30, 30, 35, 255});

    Camera2D camera = { 0 };
    camera.target = (Vector2){ 
        estadoLocal->player.position.x + estadoLocal->player.width/2.0f, 
        estadoLocal->player.position.y + estadoLocal->player.height/2.0f 
    };
    camera.offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    camera.zoom = 1.0f;

    float minX = GetScreenWidth()/2.0f; 
    float maxX = (MAP_W * TILE_SIZE) - GetScreenWidth()/2.0f;
    float minY = GetScreenHeight()/2.0f;
    float maxY = (MAP_H * TILE_SIZE) - GetScreenHeight()/2.0f;

    if (maxX < minX) camera.target.x = (MAP_W * TILE_SIZE) / 2.0f;
    else {
        if (camera.target.x < minX) camera.target.x = minX;
        if (camera.target.x > maxX) camera.target.x = maxX;
    }
    if (maxY < minY) camera.target.y = (MAP_H * TILE_SIZE) / 2.0f;
    else {
        if (camera.target.y < minY) camera.target.y = minY;
        if (camera.target.y > maxY) camera.target.y = maxY;
    }

    BeginMode2D(camera);

        // Se houver um pedido de forçar centralização da câmera (após respawn), respeite por alguns frames
        if (estadoLocal->cameraForceCenter && estadoLocal->cameraForceFrames > 0) {
            camera.target = (Vector2){
                estadoLocal->player.position.x + estadoLocal->player.width/2.0f,
                estadoLocal->player.position.y + estadoLocal->player.height/2.0f
            };
            // decrementa contador para que o efeito dure apenas alguns frames
            estadoLocal->cameraForceFrames--;
            if (estadoLocal->cameraForceFrames <= 0) {
                estadoLocal->cameraForceCenter = false;
            }
        }

        for (int y = 0; y < MAP_H; y++) {
            for (int x = 0; x < MAP_W; x++) {
                int tile = estadoLocal->map[y][x];
                if (tile == TILE_EMPTY) continue;

                int px = x * TILE_SIZE;
                int py = y * TILE_SIZE;

                switch (tile) {
                    case TILE_WALL:
                        DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, (Color){80, 80, 90, 255});
                        DrawRectangleLines(px, py, TILE_SIZE, TILE_SIZE, BLACK);
                        break;
                    case TILE_TREASURE:
                        DrawRectangle(px + 10, py + 15, TILE_SIZE - 20, TILE_SIZE - 20, GOLD);
                        DrawRectangleLines(px + 10, py + 15, TILE_SIZE - 20, TILE_SIZE - 20, ORANGE);
                        break;
                    case TILE_KEY:
                        DrawCircle(px + TILE_SIZE/2, py + TILE_SIZE/2, 10, YELLOW);
                        break;
                    case TILE_DOOR:
                        DrawRectangle(px + 5, py + 5, TILE_SIZE - 10, TILE_SIZE - 5, DARKBROWN);
                        if (!estadoLocal->hasExitKey) DrawText("X", px+15, py+10, 20, RED);
                        break;
                    case TILE_SPIKES:
                        DrawTriangle((Vector2){px+TILE_SIZE/2.0f, py+10}, (Vector2){px+5, py+TILE_SIZE}, (Vector2){px+TILE_SIZE-5, py+TILE_SIZE}, RED);
                        break;
                    case TILE_TRIANGLE_SPAWNER:
                        DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, LIGHTGRAY);
                        DrawTriangle((Vector2){px+5, py+5}, (Vector2){px+TILE_SIZE-5, py+5}, (Vector2){px+TILE_SIZE/2.0f, py+TILE_SIZE-5}, BLACK);
                        break;
                    case TILE_FALLING_TRIANGLE:
                        DrawTriangle((Vector2){px, py}, (Vector2){px+TILE_SIZE, py}, (Vector2){px+TILE_SIZE/2.0f, py+TILE_SIZE}, RED);
                        DrawTriangleLines((Vector2){px, py}, (Vector2){px+TILE_SIZE, py}, (Vector2){px+TILE_SIZE/2.0f, py+TILE_SIZE}, WHITE);
                        break;
                    case TILE_ROCK:
                        DrawCircle(px + TILE_SIZE/2, py + TILE_SIZE/2, TILE_SIZE/2 - 2, LIGHTGRAY);
                        DrawCircleLines(px + TILE_SIZE/2, py + TILE_SIZE/2, TILE_SIZE/2 - 2, BLACK);
                        break;
                    case TILE_TRIGGER:
                        DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, BROWN);
                        DrawText("!", px+15, py+10, 20, WHITE);
                        break;
                    case TILE_FIRE:
                        DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, ORANGE);
                        break;
                }
            }
        }

        // Desenhar sprite animado do personagem
        if (estadoLocal->player.spritesLoaded) {
            SpriteAnimation *currentAnim = NULL;
            
            switch (estadoLocal->player.state) {
                case PLAYER_IDLE:
                     if (estadoLocal->player.animIdle.loaded)
                        currentAnim = &estadoLocal->player.animIdle;
                    break;
                case PLAYER_MOVING:
                    if (estadoLocal->player.animMoving.loaded)
                        currentAnim = &estadoLocal->player.animMoving;
                    break;
                case PLAYER_JUMPING:
                    if (estadoLocal->player.animJumping.loaded)
                        currentAnim = &estadoLocal->player.animJumping;
                    break;
                case PLAYER_CROUCHING:
                    if (estadoLocal->player.animCrouching.loaded)
                        currentAnim = &estadoLocal->player.animCrouching;
                    break;
                case PLAYER_DEAD:
                    if (estadoLocal->player.animDead.loaded)
                        currentAnim = &estadoLocal->player.animDead;
                    break;
                default:
                    if (estadoLocal->player.animMoving.loaded)
                        currentAnim = &estadoLocal->player.animMoving;
            }
            
            if (currentAnim && currentAnim->loaded && currentAnim->frameCount > 0) {
                int frame = currentAnim->currentFrame % currentAnim->frameCount;
                Texture2D frameTex = currentAnim->frames[frame];
                
                if (frameTex.width > 0 && frameTex.height > 0) {
                    // Source: full frame texture
                    Rectangle source = {
                        0, 0,
                        estadoLocal->player.facingRight ? (float)frameTex.width : -(float)frameTex.width,
                        (float)frameTex.height
                    };
                    
                    // Destination: scale to fit player bounding box (preserve aspect, center horizontally)
                    float scale = estadoLocal->player.height / (float)frameTex.height;
                    float drawW = frameTex.width * scale;
                    Rectangle dest = {
                        estadoLocal->player.position.x + (estadoLocal->player.width - drawW) / 2.0f,
                        estadoLocal->player.position.y,
                        drawW,
                        estadoLocal->player.height
                    };
                    
                    // Se o player estiver invulnerável, desenha com efeito de piscar (alpha oscilante)
                    float alpha = 1.0f;
                    if (estadoLocal->player.invulnerable) {
                        alpha = 0.5f + 0.5f * sinf((float)GetTime() * 20.0f);
                    }
                    DrawTexturePro(frameTex, source, dest, (Vector2){0, 0}, 0.0f, Fade(WHITE, alpha));
                }
            }
            else {
                // Fallback: desenha um retângulo se não houver animação carregada
                float alpha = estadoLocal->player.invulnerable ? (0.5f + 0.5f * sinf((float)GetTime() * 20.0f)) : 1.0f;
                Color c = Fade(estadoLocal->player.color, alpha);
                DrawRectangle((int)estadoLocal->player.position.x, (int)estadoLocal->player.position.y, (int)estadoLocal->player.width, (int)estadoLocal->player.height, c);
            }
        }

    EndMode2D();

    DrawText(TextFormat("Score: %04d", estadoLocal->score), 20, 20, 20, WHITE);
    DrawRectangle(20, 50, 15, 15, GOLD);
    DrawText(TextFormat("x %d / %d", estadoLocal->collectedTreasures, estadoLocal->totalTreasures), 45, 48, 20, WHITE);
    DrawCircle(27, 85, 8, YELLOW);
    DrawText(TextFormat("x %d", estadoLocal->keys), 45, 78, 20, WHITE);
    if (estadoLocal->hasExitKey) DrawText("CHAVE MESTRA OK!", 20, 110, 20, GREEN);
    // Mostrar contador de mortes
    DrawText(TextFormat("Mortes: %d", estadoLocal->deaths), 20, 140, 20, WHITE);
}
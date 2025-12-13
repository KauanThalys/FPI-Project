#include "game.h"
#include "levels.h"
#include "input.h"
#include "raylib.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static EstadoJogo estado;

static float timerTriangulos = 0.0f;
static float timerGravidade = 0.0f;
static int nivelAtualCarregado = 1;

// --- Player ---
Player CreatePlayer(float x, float y) {
    Player p;
    p.position = (Vector2){ x, y };
    p.speed = (Vector2){ 0, 0 };
    p.width = 42.0f;
    p.height = 42.0f;
    p.color = RED;
    p.canJump = false;
    
    p.state = PLAYER_IDLE;
    p.lastState = PLAYER_IDLE;
    p.spritesLoaded = false;
    p.facingRight = true;
    
    // Inicializar animações
    SpriteAnimation emptyAnim = {.frames = NULL, .frameCount = 0, .currentFrame = 0, .frameTimer = 0.0f, .frameDuration = 0.08f, .loaded = false};
    p.animIdle = emptyAnim;
    p.animMoving = emptyAnim;
    p.animJumping = emptyAnim;
    p.animCrouching = emptyAnim;
    p.animDead = emptyAnim;
    
    // Helper para carregar animação de uma pasta com frames numerados
    #define LOAD_ANIM_FROM_DIR(anim, dir, maxFrames) do { \
        Texture2D *textures = (Texture2D*)malloc(sizeof(Texture2D) * (maxFrames)); \
        int count = 0; \
        char pathBuf[512]; \
        const char *exts[] = {".png", ".jpg", ".jpeg"}; \
        for (int f = 1; f <= (maxFrames); f++) { \
            bool found = false; \
            for (int e = 0; e < 3; e++) { \
                snprintf(pathBuf, sizeof(pathBuf), "%s/frame_%d%s", (dir), f, exts[e]); \
                if (FileExists(pathBuf)) { \
                    Texture2D tex = LoadTexture(pathBuf); \
                    if (tex.width > 0) { textures[count++] = tex; found = true; break; } \
                } \
            } \
            if (!found) break; \
        } \
        if (count > 0) { \
            (anim).frames = textures; \
            (anim).frameCount = count; \
            (anim).currentFrame = 0; \
            (anim).frameTimer = 0.0f; \
            (anim).frameDuration = 0.08f; \
            (anim).loaded = true; \
        } else { \
            free(textures); \
        } \
    } while(0)

    // Carregar animações de suas respectivas pastas
    LOAD_ANIM_FROM_DIR(p.animIdle, "assets/sprints/idle", 10);
    LOAD_ANIM_FROM_DIR(p.animMoving, "assets/sprints/moving", 10);
    LOAD_ANIM_FROM_DIR(p.animJumping, "assets/sprints/jumping", 10);
    LOAD_ANIM_FROM_DIR(p.animCrouching, "assets/sprints/crouching", 10);
    LOAD_ANIM_FROM_DIR(p.animDead, "assets/sprints/dead", 10);
    
    #undef LOAD_ANIM_FROM_DIR
    
    p.spritesLoaded = true;
    return p;
}

static bool in_map(int tx, int ty) { 
    return tx >= 0 && tx < MAP_W && ty >= 0 && ty < MAP_H; 
}

// Atualiza a animação do player
static void UpdatePlayerAnimation(Player *player, float dt) {
    if (!player || !player->spritesLoaded) return;
    
    SpriteAnimation *currentAnim = NULL;
    
    switch (player->state) {
        case PLAYER_IDLE:
            currentAnim = &player->animIdle;
            break;
        case PLAYER_MOVING:
            currentAnim = &player->animMoving;
            break;
        case PLAYER_JUMPING:
            currentAnim = &player->animJumping;
            break;
        case PLAYER_CROUCHING:
            currentAnim = &player->animCrouching;
            break;
        case PLAYER_DEAD:
            currentAnim = &player->animDead;
            break;
        default:
            // IDLE: não anima, só mostra o primeiro frame
            currentAnim = &player->animMoving;
            if (currentAnim->currentFrame != 0) {
                currentAnim->currentFrame = 0;
                currentAnim->frameTimer = 0.0f;
            }
            return;
    }
    
    if (!currentAnim || !currentAnim->loaded || currentAnim->frameCount <= 0) {
        return;
    }
    
    // Se trocou de estado, reinicia a animação
    if (player->state != player->lastState) {
        currentAnim->currentFrame = 0;
        currentAnim->frameTimer = 0.0f;
        player->lastState = player->state;
    }
    
    // Avança o timer da animação
    currentAnim->frameTimer += dt;
    if (currentAnim->frameTimer >= currentAnim->frameDuration) {
        currentAnim->frameTimer = 0.0f;
        // Advance frame but do NOT loop circularly. When reaching the last frame, hold it.
        if (currentAnim->currentFrame < currentAnim->frameCount - 1) {
            currentAnim->currentFrame++;
        } else {
            // already at last frame, keep it
            currentAnim->currentFrame = currentAnim->frameCount - 1;
        }
    }
}

static bool IsTileSolid(int tile) {
    // Se a porta for sólida, a física empurra o jogador para trás antes de detectar a interação.
    return (tile == TILE_WALL || tile == TILE_TRIGGER || tile == TILE_TRIANGLE_SPAWNER || tile == TILE_ROCK);
}

static void init_map_from_layout(const int layout[MAP_H][MAP_W]) {
    estado.totalTreasures = 0;
    estado.collectedTreasures = 0;
    estado.keys = 0;
    estado.hasExitKey = false;
    estado.score = 0;
    // reset camera control
    estado.cameraForceCenter = false;
    estado.cameraForceFrames = 0;

    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            if (layout) {
                estado.map[y][x] = layout[y][x];
                if (estado.map[y][x] == TILE_TREASURE) estado.totalTreasures++;
            } else {
                estado.map[y][x] = TILE_EMPTY;
            }
        }
    }
}

static void UpdateTraps(float dt) {
    timerTriangulos += dt;
    timerGravidade += dt;

    // Spawn Lento (3s)
    if (timerTriangulos >= 3.0f) {
        timerTriangulos = 0.0f;
        for (int y = 0; y < MAP_H - 1; y++) {
            for (int x = 0; x < MAP_W; x++) {
                if (estado.map[y][x] == TILE_TRIANGLE_SPAWNER) {
                    if (estado.map[y+1][x] == TILE_EMPTY) {
                        estado.map[y+1][x] = TILE_FALLING_TRIANGLE;
                    }
                }
            }
        }
    }

    // Gravidade 
    if (timerGravidade >= 0.45f) {
        timerGravidade = 0.0f;
        for (int y = MAP_H - 2; y >= 0; y--) {
            for (int x = 0; x < MAP_W; x++) {
                int tile = estado.map[y][x];
                if (tile == TILE_ROCK || tile == TILE_FALLING_TRIANGLE) {
                    int tileAbaixo = estado.map[y+1][x];
                    if (tileAbaixo == TILE_EMPTY) {
                        estado.map[y+1][x] = tile;
                        estado.map[y][x] = TILE_EMPTY;
                    } else if (tile == TILE_FALLING_TRIANGLE) {
                        estado.map[y][x] = TILE_EMPTY;
                    }
                }
            }
        }
    }
}

static void CheckInteractions(Rectangle playerRect) {
    int startX = (int)((playerRect.x) / TILE_SIZE);
    int endX = (int)((playerRect.x + playerRect.width) / TILE_SIZE);
    int startY = (int)((playerRect.y) / TILE_SIZE);
    int endY = (int)((playerRect.y + playerRect.height) / TILE_SIZE);

    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (!in_map(x, y)) continue;
            int tile = estado.map[y][x];

            if (tile == TILE_TREASURE) {
                estado.map[y][x] = TILE_EMPTY;
                estado.collectedTreasures++;
                estado.score += 100;
            } else if (tile == TILE_KEY) {
                estado.map[y][x] = TILE_EMPTY;
                estado.keys++;
                estado.hasExitKey = true;
            } else if (tile == TILE_TRIGGER) {
                estado.map[y][x] = TILE_EMPTY; 
                } else if (tile == TILE_SPIKES || tile == TILE_FIRE || tile == TILE_FALLING_TRIANGLE || tile == TILE_ROCK) {
                    // Se estiver invulnerável, ignora armadilhas
                    if (estado.player.invulnerable) continue;
                    // Solicita respawn para evitar reinício direto durante iteração de mapa
                    estado.respawnRequested = true;
                    return;
                }
            // PORTA: Agora acessível pois IsTileSolid retorna falso para ela
            else if (tile == TILE_DOOR) {
                if (estado.hasExitKey) {
                    if (estado.level == nivelAtualCarregado) {
                        estado.level++; // Isso dispara a transição no Update
                    }
                }
            }
        }
    }
}

void Jogo_Iniciar(void) { Jogo_IniciarFase(1); }

void Jogo_IniciarFase(int level) {
    if (level < 1) level = 1;
    estado.level = level;
    nivelAtualCarregado = level;
    timerTriangulos = 0.0f;
    timerGravidade = 0.0f;

    const int (*layout)[MAP_W] = Levels_GetLayout(level);
    init_map_from_layout(layout);

    float startX = 100, startY = 100;
    int startTileX = -1, startTileY = -1;
    bool startFound = false;
    for(int y=0; y<MAP_H; y++){
        for(int x=0; x<MAP_W; x++){
            if(estado.map[y][x] == TILE_START){
                startTileX = x; startTileY = y;
                startX = x * TILE_SIZE + (TILE_SIZE - 38)/2;
                startFound = true; break;
            }
        }
        if(startFound) break;
    }
    estado.player = CreatePlayer(startX, startY);
    // Posicionar o player sobre o tile de start (em cima, não dentro)
    if (startFound && startTileY >= 0) {
        float posY = startTileY * TILE_SIZE - estado.player.height;
        // Garantir que não saia para fora do mapa verticalmente
        float maxY = (float)(MAP_H * TILE_SIZE) - estado.player.height;
        if (posY < 0) posY = 0;
        if (posY > maxY) posY = maxY;
        estado.player.position.y = posY;
    }
    // Garantir que o player esteja dentro dos limites horizontais do mapa
    float maxXpx = (float)(MAP_W * TILE_SIZE) - estado.player.width;
    if (estado.player.position.x < 0) estado.player.position.x = 0;
    if (estado.player.position.x > maxXpx) estado.player.position.x = maxXpx;
    // Garantir estado consistente após respawn
    estado.player.speed = (Vector2){0,0};
    estado.player.state = PLAYER_IDLE;
    estado.player.lastState = PLAYER_IDLE;
    // Tornar o player invulnerável por um curto período após respawn
    estado.player.invulnerable = true;
    estado.player.invulnerableTimer = 1.0f; // 1 segundo de invulnerabilidade
    // Log utile para depuração: coordenadas de spawn
    TraceLog(LOG_INFO, "[Jogo_IniciarFase] level=%d startX=%.1f startY=%.1f", level, startX, startY);
    // Forçar recentralização da câmera por alguns frames
    estado.cameraForceCenter = true;
    estado.cameraForceFrames = 6; // cerca de 6 frames ~= 0.1s a 60fps
}

void Jogo_Atualizar(GameScreen *currentScreen) {
    float dt = GetFrameTime();
    // Processa pedido de respawn (se houver) no início do update para reiniciar a fase
    if (estado.respawnRequested) {
        estado.respawnRequested = false;
        Jogo_IniciarFase(estado.level);
        return;
    }
    
    // Transição de Fase
    if (estado.level > nivelAtualCarregado) {
        *currentScreen = LEVEL_TRANSITION; 
        return; 
    }

    UpdateTraps(dt);

    // Física 
    const float moveSpeed = 300.0f; const float accel = 2000.0f; const float friction = 1600.0f;
    const float airDrag = 500.0f; const float gravity = 1000.0f; const float jumpSpeed = 600.0f; 

    int ax = Input_GetAxisX();
    float targetVx = (float)ax * moveSpeed;
    // Ajustar direção que o player está olhando
    if (ax < 0) estado.player.facingRight = false;
    else if (ax > 0) estado.player.facingRight = true;
    float currentAccel = estado.player.canJump ? accel : (accel * 0.6f); 
    float currentFriction = estado.player.canJump ? friction : airDrag;

    if (ax != 0) {
        if (estado.player.speed.x < targetVx) {
            estado.player.speed.x += currentAccel * dt;
            if (estado.player.speed.x > targetVx) estado.player.speed.x = targetVx;
        } else if (estado.player.speed.x > targetVx) {
            estado.player.speed.x -= currentAccel * dt;
            if (estado.player.speed.x < targetVx) estado.player.speed.x = targetVx;
        }
    } else {
        if (estado.player.speed.x > 0) {
            estado.player.speed.x -= currentFriction * dt;
            if (estado.player.speed.x < 0) estado.player.speed.x = 0;
        } else if (estado.player.speed.x < 0) {
            estado.player.speed.x += currentFriction * dt;
            if (estado.player.speed.x > 0) estado.player.speed.x = 0;
        }
    }

    // Pulo: funciona ao pressionar, ou continuamente se segurando e no chão
    bool jumpInput = Input_IsJumpPressed() || Input_IsJumpHeld();
    if (jumpInput && estado.player.canJump) {
        estado.player.speed.y = -jumpSpeed;
        estado.player.canJump = false;
    }

    // Atualizar estado do personagem baseado em input/velocidade
    if (!estado.player.canJump && estado.player.speed.y != 0) {
        // Está pulando/caindo
        estado.player.state = PLAYER_JUMPING;
    } else if (estado.player.canJump && fabsf(estado.player.speed.x) > 10.0f) {
        // Está se movendo no chão
        estado.player.state = PLAYER_MOVING;
    } else {
        // Idle (parado no chão)
        estado.player.state = PLAYER_IDLE;
    }

    // Atualizar animação do player
    UpdatePlayerAnimation(&estado.player, dt);

    // Atualizar invulnerabilidade do player
    if (estado.player.invulnerable) {
        estado.player.invulnerableTimer -= dt;
        if (estado.player.invulnerableTimer <= 0.0f) {
            estado.player.invulnerable = false;
            estado.player.invulnerableTimer = 0.0f;
        }
    }

    estado.player.speed.y += gravity * dt;

    // Colisão X
    estado.player.position.x += estado.player.speed.x * dt;
    float mapWidthPx = (float)(MAP_W * TILE_SIZE);
    if (estado.player.position.x < 0) { estado.player.position.x = 0; estado.player.speed.x = 0; }
    else if (estado.player.position.x + estado.player.width > mapWidthPx) { estado.player.position.x = mapWidthPx - estado.player.width; estado.player.speed.x = 0; }

    Rectangle playerRect = { estado.player.position.x, estado.player.position.y, estado.player.width, estado.player.height };
    int startX = (int)((playerRect.x) / TILE_SIZE);
    int endX = (int)((playerRect.x + playerRect.width) / TILE_SIZE);
    int startY = (int)((playerRect.y) / TILE_SIZE);
    int endY = (int)((playerRect.y + playerRect.height - 0.1f) / TILE_SIZE);

    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (in_map(x, y) && IsTileSolid(estado.map[y][x])) {
                if (estado.player.speed.x > 0) { estado.player.position.x = x * TILE_SIZE - estado.player.width - 0.1f; } 
                else if (estado.player.speed.x < 0) { estado.player.position.x = (x + 1) * TILE_SIZE + 0.1f; }
                estado.player.speed.x = 0;
            }
        }
    }

    // Colisão Y
    estado.player.position.y += estado.player.speed.y * dt;
    playerRect.y = estado.player.position.y;
    playerRect.x = estado.player.position.x;
    estado.player.canJump = false;

    startY = (int)((playerRect.y) / TILE_SIZE);
    endY = (int)((playerRect.y + playerRect.height) / TILE_SIZE);
    startX = (int)((playerRect.x + 1.0f) / TILE_SIZE);
    endX = (int)((playerRect.x + playerRect.width - 1.0f) / TILE_SIZE);

    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {
            if (in_map(x, y) && IsTileSolid(estado.map[y][x])) {
                if (estado.player.speed.y > 0) { 
                    estado.player.position.y = y * TILE_SIZE - estado.player.height;
                    estado.player.speed.y = 0;
                    estado.player.canJump = true;
                } else if (estado.player.speed.y < 0) { 
                    estado.player.position.y = (y + 1) * TILE_SIZE;
                    estado.player.speed.y = 0;
                }
            }
        }
    }
    
    playerRect.x = estado.player.position.x;
    playerRect.y = estado.player.position.y;
    CheckInteractions(playerRect);

    if (estado.player.position.y > MAP_H * TILE_SIZE + 200) {
        estado.respawnRequested = true;
    }
}

EstadoJogo* Jogo_ObterEstado(void) { return &estado; }
void Jogo_Descarregar(void) { }

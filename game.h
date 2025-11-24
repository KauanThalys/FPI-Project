#ifndef GAME_H
#define GAME_H

#include "screens.h"
#include "raylib.h"

// --- Constantes do Mapa ---
#define TILE_SIZE 48
// CORREÇÃO AQUI: MAP_W precisa ser 60 para bater com o levels.c
#define MAP_W 60   
#define MAP_H 20   

// --- Tipos de Tiles ---
typedef enum {
    TILE_EMPTY = 0,
    TILE_WALL = 1,
    TILE_PLAYER = 2,
    TILE_TREASURE = 3,
    TILE_KEY = 4,
    TILE_DOOR = 5,
    TILE_SPIKES = 6,
    
    // --- ARMADILHAS ---
    TILE_TRIANGLE_SPAWNER = 7, 
    TILE_FALLING_TRIANGLE = 8, // <--- O "Fantasma" que te mata é o 8
    
    TILE_START = 9,
    TILE_ROCK = 10,
    TILE_TRIGGER = 11,
    TILE_FIRE = 12
} TileType;

// --- Struct Player ---
typedef struct Player {
    Vector2 position;
    Vector2 speed;
    float width;
    float height;
    Color color;
    bool canJump;     
} Player;

// --- Estado Global ---
typedef struct {
    int map[MAP_H][MAP_W]; 
    int collectedTreasures;
    int totalTreasures;
    int score;
    int level;
    int keys;
    bool hasExitKey;
    Player player; 
} EstadoJogo;

// --- Protótipos ---
Player CreatePlayer(float x, float y);
void Jogo_Iniciar(void);
void Jogo_IniciarFase(int level);
void Jogo_Atualizar(GameScreen *currentScreen);
EstadoJogo* Jogo_ObterEstado(void);
void Jogo_Descarregar(void);

#endif // GAME_H
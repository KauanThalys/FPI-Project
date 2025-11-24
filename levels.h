#ifndef LEVELS_H
#define LEVELS_H

#include "game.h"

// IDs dos níveis
typedef enum {
    // Nenhum nível definido atualmente
    LEVEL_COUNT
} LevelID;

// Retorna ponteiro para a matriz do level (MAP_H x MAP_W). Retorno NULL se não existir.
const int (*Levels_GetLayout(int level))[MAP_W];

// Retorna título do nível (NULL se não existir)
const char* Levels_GetTitle(int level);

// Retorna dica/instrução curta para o level (p.ex. tutorial hint)
const char* Levels_GetHint(int level);

#endif // LEVELS_H

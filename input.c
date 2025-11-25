// input.c
#include "input.h"
#include "raylib.h"
#include <stdbool.h>

/* Atualiza o estado de entrada por frame.
 * Placeholder: aqui você pode processar buffers, gamepad ou remapping.
 */
void Input_Update(void) {
    /* nada por enquanto */
}

/* Detecta se a tecla ESC foi pressionada neste frame. */
bool Input_IsEscapePressed(void) {
    return IsKeyPressed(KEY_ESCAPE);
}

/* Encaminha entrada do menu (delegação para screens.c). */
void Input_HandleMenu(GameScreen *currentScreen) {
    extern void UpdateMenuScreen(GameScreen *currentScreen);
    UpdateMenuScreen(currentScreen);
}

/* Eixo horizontal: -1 (esquerda), 0, +1 (direita).
 * Usa A/D ou ←/→.
 */
int Input_GetAxisX(void) {
    int ax = 0;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  ax -= 1;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) ax += 1;
    return ax;
}

/* Eixo vertical: -1 (cima), 0, +1 (baixo).
 * Usa W/S ou ↑/↓.
 *
 * Observação de design:
 * - Retornar -1 para cima facilita integrar com sistemas onde Y cresce pra baixo.
 * - Se você usar esse eixo para movimento direto, pode querer normalizar diagonais
 *   (p.ex. dividir por sqrt(2)) — posso aplicar isso depois se quiser.
 */
int Input_GetAxisY(void) {
    int ay = 0;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    ay -= 1;   // cima
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  ay += 1;   // baixo
    return ay;
}

/* Pulo: detecta pressionamento inicial (W, Espaço ou ↑). */
bool Input_IsJumpPressed(void) {
    return IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP);
}

/* Pulo segurado. */
bool Input_IsJumpHeld(void) {
    return IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP);
}

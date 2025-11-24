#include "input.h"
#include "raylib.h"

// Atualiza o estado de entrada por frame. Atualmente é um placeholder
// mas pode ser estendido para lidar com buffers, remapping ou gamepad.
void Input_Update(void) {
    // placeholder: pode processar eventos acumulados se necessário
}

// Detecta se o jogador pressionou ESC neste frame.
bool Input_IsEscapePressed(void) {
    return IsKeyPressed(KEY_ESCAPE);
}

// Trata a entrada na tela de menu delegando para a função de screens.
// Mantemos uma função wrapper para manter a separação de responsabilidades.
void Input_HandleMenu(GameScreen *currentScreen) {
    extern void UpdateMenuScreen(GameScreen *currentScreen);
    UpdateMenuScreen(currentScreen);
}

// Retorna -1/0/+1 conforme as teclas de movimento estiverem pressionadas (A/D ou ←/→).
int Input_GetAxisX(void) {
    // Usa A/D ou ←/→ para movimento horizontal
    int ax = 0;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) ax -= 1;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) ax += 1;
    return ax;
}

// Retorna true se a tecla de pulo foi pressionada neste frame (W apenas).
bool Input_IsJumpPressed(void) {
    // Pulo com W ou Espaço ou Seta pra cima
    return IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP);
}

// Retorna true se a tecla de pulo está sendo mantida pressionada (W apenas).
bool Input_IsJumpHeld(void) {
    return IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP);
}
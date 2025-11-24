#ifndef INPUT_H
#define INPUT_H

#include "screens.h"

// Atualiza o estado de entrada por frame.
// Pode ser estendido para processar buffers ou input remapeável.
void Input_Update(void);

// Retorna true se a tecla ESC foi pressionada neste frame.
bool Input_IsEscapePressed(void);

// Processa a entrada da tela de menu (cliques) e atualiza o estado atual da tela.
// Recebe ponteiro para o `GameScreen` e altera para CREDITS/GAME/EXIT conforme o botão.
void Input_HandleMenu(GameScreen *currentScreen);

// Retorna o eixo horizontal de movimento: -1 (esquerda), 0 (neutro), +1 (direita).
// Usa teclas A/D ou ←/→ atualmente.
int Input_GetAxisX(void);

// Retorna true se o botão de pulo foi pressionado neste frame (Space ou Up).
bool Input_IsJumpPressed(void);

// Retorna true se o botão de pulo está sendo mantido pressionado (Space ou Up).
bool Input_IsJumpHeld(void);

#endif // INPUT_H

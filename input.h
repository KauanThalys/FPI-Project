#ifndef INPUT_H
#define INPUT_H

#include "screens.h"
#include <stdbool.h>

// Atualiza o estado de entrada por frame.
void Input_Update(void);

// Retorna true se a tecla ESC foi pressionada neste frame.
bool Input_IsEscapePressed(void);

// Tratamento de entrada no menu
void Input_HandleMenu(GameScreen *currentScreen);

// Eixos de movimento
int Input_GetAxisX(void);
int Input_GetAxisY(void); // <- importante

// Pulo
bool Input_IsJumpPressed(void);
bool Input_IsJumpHeld(void);
// Agachar (S ou seta para baixo)
bool Input_IsCrouchHeld(void);

#endif // INPUT_H

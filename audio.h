#ifndef AUDIO_H
#define AUDIO_H

#include "screens.h" // para GameScreen
#include <stdbool.h>

/*
 Audio manager API
 - Audio_Init(): carregar músicas e SFX (assume InitAudioDevice já chamado)
 - Audio_Update(currentScreen, dt): atualizar streams e fades (chame a cada frame)
 - Audio_PlaySFX(id): tocar efeito (id: "jump","collect","hurt", ...)
 - Audio_SetMasterVolume(v): 0.0 .. 1.0
 - Audio_Unload(): descarregar tudo (antes de CloseAudioDevice)
 
 Assets esperados (coloque em assets/):
 - assets/menu_music.ogg
 - assets/game_music.ogg
 - assets/jump.wav
 - assets/collect.wav
 - assets/hurt.wav
*/

void Audio_Init(void);
void Audio_Update(GameScreen currentScreen, float dt);
void Audio_PlaySFX(const char *id);
void Audio_SetMasterVolume(float v);
void Audio_Unload(void);

#endif // AUDIO_H

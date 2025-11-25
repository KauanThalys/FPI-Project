// audio.c
#include "audio.h"
#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    Music music;
    bool loaded;
    const char *path;
} MusicEntry;

typedef struct {
    Sound sound;
    bool loaded;
    const char *path;
} SFXEntry;

/* Config */
static const float DEFAULT_MUSIC_VOLUME = 0.6f;
static const float DEFAULT_SFX_VOLUME   = 0.8f;

/* Declare apenas os paths inicialmente */
static MusicEntry musicMenu  = {0};
static MusicEntry musicGame  = {0};
static SFXEntry   sfxJump    = {0};
static SFXEntry   sfxCollect = {0};
static SFXEntry   sfxHurt    = {0};

/* Fade controller */
static Music *currentMusic = NULL;
static Music *nextMusic    = NULL;
static float masterVolume  = 1.0f;

/* Fade state */
static bool isFading      = false;
static float fadeDuration = 0.0f;
static float fadeElapsed  = 0.0f;
static float fadeFrom     = 0.0f;
static float fadeTo       = 0.0f;

/* Helper: try load music if file exists */
static void tryLoadMusic(MusicEntry *m) {
    if (m->loaded) return;
    if (FileExists(m->path)) {
        m->music = LoadMusicStream(m->path);
        m->loaded = true;
        SetMusicVolume(m->music, DEFAULT_MUSIC_VOLUME * masterVolume);
    }
}

/* Helper: try load sfx */
static void tryLoadSFX(SFXEntry *s) {
    if (s->loaded) return;
    if (FileExists(s->path)) {
        s->sound = LoadSound(s->path);
        s->loaded = true;
        SetSoundVolume(s->sound, DEFAULT_SFX_VOLUME * masterVolume);
    }
}

/* Start playing a music instantly (no fade) */
static void playMusicImmediate(MusicEntry *mentry, bool loop) {
    if (!mentry || !mentry->loaded) return;
    if (currentMusic) StopMusicStream(*currentMusic);

    currentMusic = &mentry->music;
    currentMusic->looping = loop;  // true = loop infinito, false = toca uma vez

    SetMusicVolume(*currentMusic, DEFAULT_MUSIC_VOLUME * masterVolume);
    PlayMusicStream(*currentMusic);
}

/* Fade to music */
static void fadeToMusic(MusicEntry *mentry, float durationSeconds, bool loop) {
    if (mentry) tryLoadMusic(mentry);

    fadeDuration = (durationSeconds > 0.0001f) ? durationSeconds : 0.001f;
    fadeElapsed  = 0.0f;
    isFading     = true;
    fadeFrom     = currentMusic ? DEFAULT_MUSIC_VOLUME * masterVolume : 0.0f;

    if (mentry && mentry->loaded) {
        nextMusic = &mentry->music;
        fadeTo    = DEFAULT_MUSIC_VOLUME * masterVolume;
        nextMusic->looping = loop;

        SetMusicVolume(*nextMusic, 0.0f);
        PlayMusicStream(*nextMusic);
    } else {
        nextMusic = NULL;
        fadeTo    = 0.0f;
    }
}

/* Public API */
void Audio_Init(void) {
    masterVolume = 1.0f;

    // Definir paths
    musicMenu.path  = "assets/menu_music.ogg";   // 
    musicGame.path  = "assets/game_music.ogg";   // 
    sfxJump.path    = "assets/jump.ogg";        // 
    sfxCollect.path = "assets/collect.wav";      // 
    sfxHurt.path    = "assets/hurt.wav";         // 


    // Carregar
    tryLoadMusic(&musicMenu);
    tryLoadMusic(&musicGame);
    tryLoadSFX(&sfxJump);
    tryLoadSFX(&sfxCollect);
    tryLoadSFX(&sfxHurt);

    currentMusic = NULL;
    nextMusic    = NULL;
    isFading     = false;
}

void Audio_Update(GameScreen currentScreen, float dt) {
    if (musicMenu.loaded) UpdateMusicStream(musicMenu.music);
    if (musicGame.loaded) UpdateMusicStream(musicGame.music);

    MusicEntry *desired = NULL;
    if (currentScreen == MENU) desired = &musicMenu;
    else if (currentScreen == GAME) desired = &musicGame;

    if (desired) tryLoadMusic(desired);

    if (!isFading) {
        if (desired && (!currentMusic || currentMusic != &desired->music)) {
            fadeToMusic(desired, 0.6f, true);
        } else if (!desired && currentMusic) {
            fadeToMusic(NULL, 0.6f, false);
        }
    }

    if (isFading) {
        fadeElapsed += dt;
        float t = fadeElapsed / fadeDuration;
        if (t > 1.0f) t = 1.0f;

        float volFrom = fadeFrom * (1.0f - t);
        float volTo   = fadeTo   * t;

        if (currentMusic) SetMusicVolume(*currentMusic, volFrom);
        if (nextMusic)    SetMusicVolume(*nextMusic, volTo);

        if (t >= 1.0f) {
            if (currentMusic && nextMusic && currentMusic != nextMusic)
                StopMusicStream(*currentMusic);
            currentMusic = nextMusic;
            nextMusic    = NULL;
            isFading     = false;
            fadeElapsed  = 0.0f;
        }
    }

    if (!isFading && currentMusic)
        SetMusicVolume(*currentMusic, DEFAULT_MUSIC_VOLUME * masterVolume);

    if (currentMusic) UpdateMusicStream(*currentMusic);
    if (nextMusic)    UpdateMusicStream(*nextMusic);
}

void Audio_PlaySFX(const char *id) {
    if (!id) return;
    if (strcmp(id, "jump") == 0 && sfxJump.loaded) PlaySound(sfxJump.sound);
    else if (strcmp(id, "collect") == 0 && sfxCollect.loaded) PlaySound(sfxCollect.sound);
    else if (strcmp(id, "hurt") == 0 && sfxHurt.loaded) PlaySound(sfxHurt.sound);
}

void Audio_SetMasterVolume(float v) {
    if (v < 0.0f) v = 0.0f;
    if (v > 1.0f) v = 1.0f;
    masterVolume = v;

    if (musicMenu.loaded)  SetMusicVolume(musicMenu.music, DEFAULT_MUSIC_VOLUME * masterVolume);
    if (musicGame.loaded)  SetMusicVolume(musicGame.music, DEFAULT_MUSIC_VOLUME * masterVolume);
    if (sfxJump.loaded)    SetSoundVolume(sfxJump.sound, DEFAULT_SFX_VOLUME * masterVolume);
    if (sfxCollect.loaded) SetSoundVolume(sfxCollect.sound, DEFAULT_SFX_VOLUME * masterVolume);
    if (sfxHurt.loaded)    SetSoundVolume(sfxHurt.sound, DEFAULT_SFX_VOLUME * masterVolume);
}

void Audio_Unload(void) {
    if (musicMenu.loaded)  { StopMusicStream(musicMenu.music);  UnloadMusicStream(musicMenu.music);  musicMenu.loaded  = false; }
    if (musicGame.loaded)  { StopMusicStream(musicGame.music);  UnloadMusicStream(musicGame.music);  musicGame.loaded  = false; }

    if (sfxJump.loaded)    { UnloadSound(sfxJump.sound);         sfxJump.loaded    = false; }
    if (sfxCollect.loaded) { UnloadSound(sfxCollect.sound);      sfxCollect.loaded = false; }
    if (sfxHurt.loaded)    { UnloadSound(sfxHurt.sound);         sfxHurt.loaded    = false; }

    currentMusic = NULL;
    nextMusic    = NULL;
    isFading     = false;
}

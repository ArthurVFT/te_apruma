// audio.c - musica de fundo e efeitos sonoros (Raylib).

#include "audio.h"
#include "raylib.h"

static Sound winSound;
static Sound gameOverSound;
static Music music;

void AudioLoad(void) {
    InitAudioDevice();

    winSound      = LoadSound("assets/audio/win.wav");
    gameOverSound = LoadSound("assets/audio/death.wav");

    music = LoadMusicStream("assets/audio/background.wav");
    SetMusicVolume(music, 0.2f);   // musica mais baixa que os efeitos
    PlayMusicStream(music);        // a musica repete sozinha
}

void AudioUpdate(void) {
    UpdateMusicStream(music);
}

void AudioPlayWin(void)      { PlaySound(winSound); }
void AudioPlayGameOver(void) { PlaySound(gameOverSound); }

void AudioUnload(void) {
    UnloadSound(winSound);
    UnloadSound(gameOverSound);
    UnloadMusicStream(music);
    CloseAudioDevice();
}
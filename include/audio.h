// audio.h - musica de fundo e efeitos sonoros.

#ifndef AUDIO_H
#define AUDIO_H

void AudioLoad(void);         // abre o dispositivo e carrega os sons
void AudioUpdate(void);       // chamar TODO frame (mantem a musica tocando)
void AudioPlayWin(void);      // efeito de fase concluida
void AudioPlayGameOver(void); // efeito de game over
void AudioUnload(void);       // libera os sons e fecha o dispositivo

#endif
#ifndef TEXTURES_H
#define TEXTURES_H

#include "raylib.h"
#include <stdbool.h>

// Folhas de sprites: quadros de 32x32 empilhados na vertical.
#define DOG_FRAME_W       32
#define DOG_FRAME_H       32
#define DOG_FRAME_COUNT    7

#define BAG_FRAME_W       32
#define BAG_FRAME_H       32
#define BAG_FRAME_COUNT    4

#define PLAYER_FRAME_W    32
#define PLAYER_FRAME_H    32
#define PLAYER_FRAME_COUNT 7

void TexturesLoad(void);      // chamar DEPOIS do InitWindow
void TexturesUnload(void);    // chamar ANTES do CloseWindow

// Cachorro
bool      TexturesDogLoaded(void);
Texture2D TexturesGetDog(void);

// Sacola de lixo
bool      TexturesBagLoaded(void);
Texture2D TexturesGetBag(void);

// Jogador (Italo)
bool      TexturesPlayerLoaded(void);
Texture2D TexturesGetPlayer(void);

#endif

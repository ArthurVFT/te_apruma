#ifndef TEXTURES_H
#define TEXTURES_H

#include "raylib.h"
#include <stdbool.h>

#define DOG_FRAME_W      32
#define DOG_FRAME_H      32
#define DOG_FRAME_COUNT   7

void TexturesLoad(void);      // chamar DEPOIS do InitWindow
void TexturesUnload(void);    // chamar ANTES do CloseWindow
bool TexturesDogLoaded(void);
Texture2D TexturesGetDog(void);

#endif
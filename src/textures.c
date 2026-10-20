#include "textures.h"

static Texture2D dogTexture;   // static zera tudo: id == 0 significa "nao carregou"

void TexturesLoad(void) {
    dogTexture = LoadTexture("assets/sprites/cachorro_andando.png");

    if (dogTexture.id == 0) {
        TraceLog(LOG_WARNING, "Sprite do cachorro nao encontrada; usando retangulos.");
        return;
    }
    SetTextureFilter(dogTexture, TEXTURE_FILTER_POINT);  // pixel art nitida ao ampliar
}

void TexturesUnload(void) {
    if (dogTexture.id != 0) {
        UnloadTexture(dogTexture);
        dogTexture.id = 0;
    }
}

bool TexturesDogLoaded(void) { return dogTexture.id != 0; }
Texture2D TexturesGetDog(void) { return dogTexture; }
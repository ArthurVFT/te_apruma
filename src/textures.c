#include "textures.h"

// static zera tudo: id == 0 significa "nao carregou"
static Texture2D dogTexture;
static Texture2D bagTexture;
static Texture2D playerTexture;

// Carrega uma textura e deixa a pixel art nitida ao ampliar.
// Retorna false (e avisa no log) se o arquivo nao for encontrado.
static bool LoadPixelTexture(Texture2D* texture, const char* path) {
    *texture = LoadTexture(path);
    if (texture->id == 0) {
        TraceLog(LOG_WARNING, "Sprite nao encontrada: %s; usando retangulos.", path);
        return false;
    }
    SetTextureFilter(*texture, TEXTURE_FILTER_POINT);
    return true;
}

void TexturesLoad(void) {
    LoadPixelTexture(&dogTexture,    "assets/sprites/cachorro_andando.png");
    LoadPixelTexture(&bagTexture,    "assets/sprites/SacoDeLixo_3.png");
    LoadPixelTexture(&playerTexture, "assets/sprites/italo.png");
}

void TexturesUnload(void) {
    if (dogTexture.id    != 0) { UnloadTexture(dogTexture);    dogTexture.id = 0; }
    if (bagTexture.id    != 0) { UnloadTexture(bagTexture);    bagTexture.id = 0; }
    if (playerTexture.id != 0) { UnloadTexture(playerTexture); playerTexture.id = 0; }
}

bool TexturesDogLoaded(void)    { return dogTexture.id    != 0; }
Texture2D TexturesGetDog(void)  { return dogTexture; }

bool TexturesBagLoaded(void)    { return bagTexture.id    != 0; }
Texture2D TexturesGetBag(void)  { return bagTexture; }

bool TexturesPlayerLoaded(void) { return playerTexture.id != 0; }
Texture2D TexturesGetPlayer(void) { return playerTexture; }

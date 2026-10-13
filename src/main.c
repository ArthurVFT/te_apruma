#if __has_include("raylib.h")
    #include "raylib.h"
#elif __has_include("C:/raylib/src/raylib.h")
    #include "C:/raylib/src/raylib.h"
#else
    #include "raylib.h"
#endif

#include "game.h"

int main(void) {
    GameInit();
    GameRun();
    GameClose();
    return 0;
}
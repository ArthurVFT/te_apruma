#if __has_include("raylib.h")
    #include "raylib.h"
#elif __has_include("C:/raylib/src/raylib.h")
    #include "C:/raylib/src/raylib.h"
#elif __has_include("C:/raylib/include/raylib.h")
    #include "C:/raylib/include/raylib.h"
#elif __has_include("C:/raylib/raylib/src/raylib.h")
    #include "C:/raylib/raylib/src/raylib.h"
#elif __has_include("C:/raylib/raylib/include/raylib.h")
    #include "C:/raylib/raylib/include/raylib.h"
#elif __has_include("C:/Users/arthu/source/repos/raylib/src/raylib.h")
    #include "C:/Users/arthu/source/repos/raylib/src/raylib.h"
#elif __has_include("C:/Users/arthu/source/repos/raylib/include/raylib.h")
    #include "C:/Users/arthu/source/repos/raylib/include/raylib.h"
#else
    #error "raylib.h not found. Install raylib or add the correct include path."
#endif

int main(void) {
    InitWindow(800, 450, "Te Apruma!");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Raylib funcionando!", 250, 200, 30, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
#include "raylib.h"
#include "Renderer.h"
#include "CustomCamera.h"
#include <iostream>

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
   
    int screenWidth = 800;
    int screenHeight = 800;
    char fpsText[10];

    InitWindow(screenWidth, screenHeight, "raytracing in raylib");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    Renderer m_Renderer;
    CustomCamera m_Camera(45.0f, 0.1f, 100.0f);
    //--------------------------------------------------------------------------------------
    m_Camera.OnResize();
    m_Renderer.OnResize(m_Camera);
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsWindowResized()) {
            m_Camera.OnResize();
            m_Renderer.OnResize(m_Camera);
        }
        if (IsKeyPressed(KEY_RIGHT))
            m_Renderer.ChangeColor();
        int fps = GetFPS();
        sprintf(fpsText, "%d FPS", fps);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        m_Renderer.Render(m_Camera);
        m_Camera.OnUpdate(GetFrameTime());

        DrawText(fpsText, 10, 10, 30, WHITE);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    // TODO: Unload all loaded resources at this point

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
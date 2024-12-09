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
    Scene m_Scene;

    {
        Sphere sphere;
        m_Scene.Spheres.push_back(sphere);
    }
    {
        Sphere sphere;
        sphere.Position = { 0.25f, 0.1f, -0.25f };
        sphere.Albedo = { 0.5f, 0.1f, 0.3f };
        m_Scene.Spheres.push_back(sphere);
    }
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsWindowResized()) {
            m_Camera.OnResize();
            m_Renderer.OnResize();
        }
        
        //Inputs
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_RIGHT)) {
            m_Scene.Spheres[0].Position += Vector3UnitX * 0.01f;
            m_Renderer.OnSphereMove();
        }
        if (IsKeyDown(KEY_LEFT)) {
            m_Scene.Spheres[0].Position -= Vector3UnitX * 0.01f;
            m_Renderer.OnSphereMove();
        }
        if (IsKeyDown(KEY_UP)) {
            m_Scene.Spheres[0].Position += Vector3UnitY * 0.01f;
            m_Renderer.OnSphereMove();
        }
        if (IsKeyDown(KEY_DOWN)) {
            m_Scene.Spheres[0].Position -= Vector3UnitY * 0.01f;
            m_Renderer.OnSphereMove();
        }
        if (IsKeyDown(KEY_ONE)) {
            m_Scene.Spheres[0].Albedo = { ColorNormalize(WHITE).x,ColorNormalize(WHITE).y,ColorNormalize(WHITE).z };
            m_Renderer.OnSphereMove();
        }
        if (IsKeyDown(KEY_TWO)) {
            m_Scene.Spheres[0].Albedo = { ColorNormalize(RED).x,ColorNormalize(RED).y,ColorNormalize(RED).z };
            m_Renderer.OnSphereMove();
        }
        if (IsKeyDown(KEY_THREE)) {
            m_Scene.Spheres[0].Albedo = { ColorNormalize(BLUE).x,ColorNormalize(BLUE).y,ColorNormalize(BLUE).z };
            m_Renderer.OnSphereMove();
        }
        if (IsKeyDown(KEY_FOUR)) {
            m_Scene.Spheres[0].Albedo = { ColorNormalize(GREEN).x,ColorNormalize(GREEN).y,ColorNormalize(GREEN).z };
            m_Renderer.OnSphereMove();
        }
        if (IsKeyDown(KEY_FIVE)) {
            m_Scene.Spheres[0].Albedo = { ColorNormalize(VIOLET).x,ColorNormalize(VIOLET).y,ColorNormalize(VIOLET).z };
            m_Renderer.OnSphereMove();
        }
        if (IsKeyDown(KEY_EQUAL)) {
            m_Scene.Spheres[0].Radius += 0.01f;
            m_Renderer.OnSphereMove();
        }
        if (IsKeyDown(KEY_MINUS)) {
            m_Scene.Spheres[0].Radius -= 0.01f;
            m_Renderer.OnSphereMove();
        }
        //----------------------------------------------------------------------------------

        int fps = GetFPS();
        sprintf(fpsText, "%d FPS", fps);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        m_Renderer.Render(m_Scene, m_Camera);
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
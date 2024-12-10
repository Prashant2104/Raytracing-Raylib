#include "raylib.h"
#include "Renderer.h"
#include "CustomCamera.h"
#include <iostream>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

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
    bool showMessageBox = false;

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

    int selectedSphere = 0;
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
            m_Scene.Spheres[selectedSphere].Position += Vector3UnitX * 0.01f;
        }
        if (IsKeyDown(KEY_LEFT)) {
            m_Scene.Spheres[selectedSphere].Position -= Vector3UnitX * 0.01f;
        }
        if (IsKeyDown(KEY_UP)) {
            m_Scene.Spheres[selectedSphere].Position += Vector3UnitY * 0.01f;
        }
        if (IsKeyDown(KEY_DOWN)) {
            m_Scene.Spheres[selectedSphere].Position -= Vector3UnitY * 0.01f;
        }
        if (IsKeyDown(KEY_ONE)) {
            m_Scene.Spheres[selectedSphere].Albedo = { ColorNormalize(WHITE).x,ColorNormalize(WHITE).y,ColorNormalize(WHITE).z };
        }
        if (IsKeyDown(KEY_TWO)) {
            m_Scene.Spheres[selectedSphere].Albedo = { ColorNormalize(RED).x,ColorNormalize(RED).y,ColorNormalize(RED).z };
        }
        if (IsKeyDown(KEY_THREE)) {
            m_Scene.Spheres[selectedSphere].Albedo = { ColorNormalize(BLUE).x,ColorNormalize(BLUE).y,ColorNormalize(BLUE).z };
        }
        if (IsKeyDown(KEY_FOUR)) {
            m_Scene.Spheres[selectedSphere].Albedo = { ColorNormalize(GREEN).x,ColorNormalize(GREEN).y,ColorNormalize(GREEN).z };
        }
        if (IsKeyDown(KEY_FIVE)) {
            m_Scene.Spheres[selectedSphere].Albedo = { ColorNormalize(VIOLET).x,ColorNormalize(VIOLET).y,ColorNormalize(VIOLET).z };
        }
        if (IsKeyDown(KEY_EQUAL)) {
            m_Scene.Spheres[selectedSphere].Radius += 0.01f;
        }
        if (IsKeyDown(KEY_MINUS)) {
            m_Scene.Spheres[selectedSphere].Radius -= 0.01f;
        }
        //----------------------------------------------------------------------------------
        m_Renderer.OnSphereMove();

        int fps = GetFPS();
        sprintf(fpsText, "%d FPS", fps);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(GetColor(0x0f0f0fff));
        GuiGetStyle(DEFAULT, BACKGROUND_COLOR);
        m_Renderer.Render(m_Scene, m_Camera);
        m_Camera.OnUpdate(GetFrameTime());

        GuiSpinner(Rectangle{ 75, 50, 100, 25 }, "SelectSphere", &selectedSphere, 0, 1, false);
        //GuiColorPicker(Rectangle{ 50, 50, 100, 50 }, "Sphere Color", &ColorFromNormalized(m_Scene.Spheres[selectedSphere].Albedo));

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
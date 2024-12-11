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
    int screenWidth = 750;
    int screenHeight = 750;
    char fpsText[10];
    bool showMessageBox = false;

    InitWindow(screenWidth, screenHeight, "raytracing in raylib");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    CustomCamera m_Camera(45.0f, 0.1f, 100.0f);
    Scene m_Scene;

    Mat& pinkSphere = m_Scene.Materials.emplace_back();
    pinkSphere.Albedo = ColorFromNormalized({ 1.0f, 0.0f, 1.0f, 1.0f });
    pinkSphere.Roughness = 0.05f;

    Mat& blueSphere = m_Scene.Materials.emplace_back();
    blueSphere.Albedo = ColorFromNormalized({ 0.2f, 0.3f, 1.0f, 1.0f });
    blueSphere.Roughness = 0.1f;

    {
        Sphere sphere;
        sphere.MaterialID = 0;
        m_Scene.Spheres.push_back(sphere);
    }
    {
        Sphere sphere;
        sphere.Position = { 0.0f, -100.55f, 0.0f };
        sphere.Radius = 100.0f;
        sphere.MaterialID = 1;
        m_Scene.Spheres.push_back(sphere);
    }

    Renderer m_Renderer(m_Scene, m_Camera);

    int selectedMaterial = 0;
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
        /*if (IsKeyDown(KEY_RIGHT)) {
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
        if (IsKeyDown(KEY_COMMA)) {
            m_Scene.Spheres[selectedSphere].Position += Vector3UnitZ * 0.01f;
        }
        if (IsKeyDown(KEY_APOSTROPHE)) {
            m_Scene.Spheres[selectedSphere].Position -= Vector3UnitZ * 0.01f;
        }
        if (IsKeyDown(KEY_EQUAL)) {
            m_Scene.Spheres[selectedSphere].Radius += 0.01f;
        }
        if (IsKeyDown(KEY_MINUS)) {
            m_Scene.Spheres[selectedSphere].Radius -= 0.01f;
        }*/
        if (IsKeyPressed(KEY_SPACE)) {
            m_Renderer.OnSphereMove();
        }
        //----------------------------------------------------------------------------------

        int fps = GetFPS();
        sprintf(fpsText, "%d FPS", fps);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(GetColor(0x0f0f0fff));
        GuiGetStyle(DEFAULT, BACKGROUND_COLOR);
        m_Renderer.Render();
        m_Camera.OnUpdate(GetFrameTime());

        GuiSpinner(Rectangle{ 75, 50, 100, 25 }, "Material ID", &selectedMaterial, 0, m_Scene.Materials.size() - 1, false);
        GuiSlider(Rectangle{ 75, 100, 100, 25 }, "MinRough", "MaxRough", &m_Scene.Materials[selectedMaterial].Roughness, 0.0f, 1.0f);
        GuiColorPicker(Rectangle{ 75, 150, 100, 50 }, "SphereColor", &m_Scene.Materials[selectedMaterial].Albedo);

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
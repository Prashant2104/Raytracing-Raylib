#include "Renderer.h"
#include "CustomCamera.h"
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

    InitWindow(screenWidth, screenHeight, "raytracing in raylib");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    CustomCamera m_Camera(45.0f, 0.1f, 100.0f);
    Scene m_Scene;

    Mat& s1 = m_Scene.Materials.emplace_back();
    s1.Albedo = ColorFromNormalized({ 1.0f, 0.0f, 1.0f, 1.0f });
    s1.Roughness = 0.2f;

    Mat& s2 = m_Scene.Materials.emplace_back();
    s2.Albedo = ColorFromNormalized({ 0.10f, 0.90f, 0.70f, 1.0f });
    s2.Roughness = 0.1f;

    Mat& sF = m_Scene.Materials.emplace_back();
    sF.Albedo = ColorFromNormalized({ 0.2f, 0.3f, 1.0f, 1.0f });
    sF.Roughness = 0.75f;

    {
        Sphere sphere;
        sphere.MaterialID = 0;
        m_Scene.Spheres.push_back(sphere);
    }
    {
        Sphere sphere;
        sphere.Position = { 1.0f, 0.2, -1.0f };
        sphere.Radius = 0.7f;
        sphere.MaterialID = 1;
        m_Scene.Spheres.push_back(sphere);
    }
    {
        Sphere sphere;
        sphere.Position = { 0.0f, -100.55f, 0.0f };
        sphere.Radius = 100.0f;
        sphere.MaterialID = 2;
        m_Scene.Spheres.push_back(sphere);
    }

    Renderer m_Renderer(m_Scene, m_Camera);

    int selectedMaterial = 0;
    char name[128] = "Test.png";
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
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(GetColor(0x0f0f0fff));
        GuiGetStyle(DEFAULT, BACKGROUND_COLOR);

        m_Renderer.Render();
        m_Camera.OnUpdate(GetFrameTime());

        GuiSpinner(Rectangle{ 50, 50, 100, 25 }, "S ID ", &selectedMaterial, 0, m_Scene.Materials.size() - 1, false);
        GuiSlider(Rectangle{ 50, 100, 100, 25 }, "R Min", "R Max", &m_Scene.Materials[selectedMaterial].Roughness, 0.0f, 1.0f);
        GuiColorPicker(Rectangle{ 50, 150, 100, 50 }, "SphereColor", &m_Scene.Materials[selectedMaterial].Albedo);
        if (GuiButton(Rectangle{ 50, 225, 100, 30 }, "Render")) {
            m_Renderer.ExportRender(name);
        }
        GuiTextInputBox(Rectangle{ 50, 275, 125, 150 }, "Image name", "What should the\nimage be called?", "Save Name", name, 25, false);
        DrawFPS(25, 10);
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
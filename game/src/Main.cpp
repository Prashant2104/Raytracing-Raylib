#include "Renderer.h"
#include "CustomCamera.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    Scene SetupScene(void);
    // Initialization
    //--------------------------------------------------------------------------------------   
    int screenWidth = 1200;
    int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "raytracing in raylib");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    CustomCamera m_Camera(45.0f, 0.1f, 100.0f);
    Scene m_Scene = SetupScene();
    Renderer m_Renderer(m_Scene, m_Camera);

    int selectedMaterial = 0;
    char name[128] = "Test.png";
    bool drawGui = true;
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
        if (m_Camera.OnUpdate(GetFrameTime())) {
            m_Renderer.ResetFrameIndex();
        }

        GuiCheckBox(Rectangle{ (float)GetScreenWidth() - 90, 50, 25, 25 }, "Show GUI", &drawGui);

        if (drawGui) {
            GuiPanel(Rectangle{ 10, 10, 200, 550 }, "UI");
            GuiSpinner(Rectangle{ 50, 50, 100, 25 }, "S ID ", &selectedMaterial, 0, m_Scene.Materials.size() - 1, false);
            GuiSlider(Rectangle{ 50, 100, 100, 25 }, "R Min", "R Max", &m_Scene.Materials[selectedMaterial].Roughness, 0.0f, 1.0f);
            GuiColorPicker(Rectangle{ 50, 150, 100, 50 }, "SphereColor", &m_Scene.Materials[selectedMaterial].Albedo);
            if (GuiButton(Rectangle{ 50, 225, 100, 30 }, "Render")) {
                m_Renderer.ExportRender(name);
            }
            GuiTextInputBox(Rectangle{ 50, 275, 125, 150 }, "Image name", "What should the\nimage be called?", "Save Name", name, 25, false);
            GuiCheckBox(Rectangle{ 50, 450, 30, 30 }, "Accumulate", &m_Renderer.GetSettings().accumulate);
            if (GuiButton(Rectangle{ 50, 500, 100, 30 }, "Reset")) {
                m_Renderer.ResetFrameIndex();
            }
        }
        DrawFPS(GetScreenWidth()-90, 10);

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

Scene SetupScene(void) {
    Scene m_Scene;
    {
        Mat& s = m_Scene.Materials.emplace_back();
        s.Albedo = ColorFromNormalized({ 0.2f, 0.3f, 1.0f, 1.0f });
        s.Roughness = 0.05f;
    }
    {
        Mat& s = m_Scene.Materials.emplace_back();
        s.Albedo = ColorFromNormalized({ 1.0f, 0.0f, 1.0f, 1.0f });
        s.Roughness = 0.82f;
    }
    {
        Mat& s = m_Scene.Materials.emplace_back();
        s.Albedo = ColorFromNormalized({ 0.10f, 0.90f, 0.70f, 1.0f });
        s.Roughness = 0.61f;
    }
    {
        Mat& s = m_Scene.Materials.emplace_back();
        s.Albedo = ColorFromNormalized({ 0.50f, 0.10f, 0.80f, 1.0f });
        s.Roughness = 0.7f;
    }
    {
        Mat& s = m_Scene.Materials.emplace_back();
        s.Albedo = ColorFromNormalized({ 0.75f, 0.91f, 0.58f, 1.0f });
        s.Roughness = 0.55f;
    }
    {
        Mat& s = m_Scene.Materials.emplace_back();
        s.Albedo = ColorFromNormalized({ 0.58f, 0.51f, 0.81f, 1.0f });
        s.Roughness = 0.75f;
    }
    {
        Mat& s = m_Scene.Materials.emplace_back();
        s.Albedo = ColorFromNormalized({ 0.58f, 0.751f, 0.81f, 1.0f });
        s.Roughness = 0.95f;
    }

    {
        Sphere sphere;
        sphere.Position = { 0.0f, -100.55f, 0.0f };
        sphere.Radius = 100.0f;
        sphere.MaterialID = 0;
        m_Scene.Spheres.push_back(sphere);
    }

    {
        Sphere sphere;
        sphere.MaterialID = 1;
        m_Scene.Spheres.push_back(sphere);
    }
    {
        Sphere sphere;
        sphere.Position = { 1.0f, 0.15f, -1.0f };
        sphere.Radius = 0.7f;
        sphere.MaterialID = 2;
        m_Scene.Spheres.push_back(sphere);
    }
    {
        Sphere sphere;
        sphere.Position = { -1.0f, 0.05f, 0.50f };
        sphere.Radius = 0.58f;
        sphere.MaterialID = 3;
        m_Scene.Spheres.push_back(sphere);
    }
    {
        Sphere sphere;
        sphere.Position = { 1.0f, 1.5f, -1.0f };
        sphere.Radius = 0.8f;
        sphere.MaterialID = 4;
        m_Scene.Spheres.push_back(sphere);
    }
    {
        Sphere sphere;
        sphere.Position = { -1.75f, 0.5f, -1.0f };
        sphere.Radius = 0.8f;
        sphere.MaterialID = 5;
        m_Scene.Spheres.push_back(sphere);
    }
    {
        Sphere sphere;
        sphere.Position = { 2.0f, 0.05f, -1.5f };
        sphere.Radius = 0.3f;
        sphere.MaterialID = 6;
        m_Scene.Spheres.push_back(sphere);
    }
    return m_Scene;
}
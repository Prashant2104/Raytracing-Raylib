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
    char name[128] = "Render.png";
    bool drawGui = false;
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
        if (!drawGui) {
            if (m_Camera.OnUpdate(GetFrameTime())) {
                m_Renderer.ResetFrameIndex();
            }
        }

        if (drawGui) {
            GuiPanel(Rectangle{ 10, 10, 210, 625 }, "UI");
            GuiSpinner(Rectangle{ 50, 50, 120, 25 }, "S ID ", &selectedMaterial, 0, m_Scene.Materials.size() - 1, false);
            GuiColorPicker(Rectangle{ 50, 90, 100, 100 }, "SphereColor", &m_Scene.Materials[selectedMaterial].Albedo);
            GuiSlider(Rectangle{50, 205, 120, 25}, "R Min", "R Max", &m_Scene.Materials[selectedMaterial].Roughness, 0.0f, 1.0f);
            GuiColorPicker(Rectangle{ 50, 250, 100, 100 }, "SphereColor", &m_Scene.Materials[selectedMaterial].EmissionColor);
            GuiSlider(Rectangle{ 50, 365, 120, 25 }, "E Min", "E Max", &m_Scene.Materials[selectedMaterial].EmissionPower, 0.0f, 50.0f);
            GuiTextBox(Rectangle{ 50, 410, 120, 30 }, name, 25, true);
            if (GuiButton(Rectangle{ 50, 460, 120, 30 }, "Render")) {
                m_Renderer.ExportRender(name);
            }
            //GuiTextInputBox(Rectangle{ 50, 460, 120, 120 }, "Image name", "Set Render Name", "Save Name", name, 25, false);
            GuiCheckBox(Rectangle{ 50, 505, 25, 25 }, "Accumulate", &m_Renderer.GetSettings().accumulate);
            if (GuiButton(Rectangle{ 50, 550, 120, 30 }, "Reset")) {
                m_Renderer.ResetFrameIndex();
            }
            DrawText("*Close GUI to move", 50, 600, 15, MAROON);
        }
        DrawFPS(GetScreenWidth() - 100, 15);
        GuiCheckBox(Rectangle{ (float)GetScreenWidth() - 100, 50, 25, 25 }, "Show GUI", &drawGui);

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
        s.Albedo = ColorFromNormalized({ 0.8f, 0.5f, 0.20f, 1.0f });
        s.Roughness = 0.25f;
        s.EmissionColor = s.Albedo;
        s.EmissionPower = 15.0f;
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
        sphere.Position = { 0.0f, 0.0f, 1.5f };
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
        sphere.Position = { 3.5f, 5.5f, -2.2f };
        sphere.Radius = 1.0f;
        sphere.MaterialID = 4;
        m_Scene.Spheres.push_back(sphere);
    }
    return m_Scene;
}
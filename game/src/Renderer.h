#pragma once
#include "raylib.h"
#include "CustomCamera.h"
#include <iostream>

class Renderer {
public:
    Renderer();

    void OnResize(const CustomCamera& camera);
    void Render(const CustomCamera& camera);
    void ChangeColor();

private:
    Vector4 TraceRay(const Ray& ray);

private:
    Image m_FinalImage;
    Texture2D m_Texture2D;
    Vector3 m_SphereColor;

    int m_ColorIndex;
    int m_ScreenWidth;
    int m_ScreenHeight;
};
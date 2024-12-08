#pragma once
#include "raylib.h"
#include <iostream>

class Renderer {
public:
    Renderer();

    void OnResize();
    void Render() const;
    void ChangeColor();

private:
    Vector4 TraceRay(Vector2 coord);

private:
    Image m_FinalImage;
    Texture2D m_Texture2D;
    Vector3 m_SphereColor;

    int m_ColorIndex = 0;
    int m_ScreenWidth;
    int m_ScreenHeight;
};
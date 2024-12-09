#pragma once
#include "raylib.h"
#include "CustomCamera.h"
#include "Scene.h"

class Renderer {
public:
    Renderer();

    void OnResize();
    void OnSphereMove();
    void Render(const Scene& scene, const CustomCamera& camera);
    Vector4 TraceRay(const Scene& scene, const Ray& ray) const;
private:
    void UpdateTextureBuffer(const Scene& scene, const CustomCamera& camera);
private:
    Image m_FinalImage;
    Texture2D m_Texture2D;
    Vector3 m_SphereColor;

    int m_ScreenWidth;
    int m_ScreenHeight;

    bool m_InitialRenderPass = false;
};
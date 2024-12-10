#pragma once
#include "raylib.h"
#include "CustomCamera.h"
#include "Scene.h"

class Renderer {
public:
    Renderer(const Scene& scene, const CustomCamera& camera);

    void OnResize();
    void OnSphereMove();
    void Render();
private:
    struct HitPayload {
        float HitDistance;
        Vector3 WorldPosition;
        Vector3 WorldNormal;

        int ObjectIndex;
    };

    Vector4 PerPixel(uint32_t x, uint32_t y);

    HitPayload TraceRay(const Ray& ray) const;
    HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex) const;
    HitPayload Miss(const Ray& ray) const;

    void UpdateTextureBuffer();
private:
    Image m_FinalImage;
    Texture2D m_Texture2D;
    Vector3 m_SphereColor;

    const Scene* m_ActiveScene = nullptr;
    const CustomCamera* m_ActiveCamera = nullptr;
    int m_ScreenWidth;
    int m_ScreenHeight;

    bool m_InitialRenderPass = false;
};
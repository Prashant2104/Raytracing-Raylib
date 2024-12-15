#pragma once
#include "raylib.h"
#include "CustomCamera.h"
#include "Scene.h"

class Renderer {
public:
    struct Settings
    {
        bool accumulate = true;
    };
public:
    Renderer(const Scene& scene, const CustomCamera& camera);

    void OnResize();
    void ExportRender(const char* name) const;
    void Render();
    void ResetFrameIndex() { m_FrameIndex = 1; }
    Settings& GetSettings() { return m_Settings; }
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
    Vector4* m_AccumulationData = nullptr;
    uint32_t m_FrameIndex = 1;
    Settings m_Settings;

    std::vector<uint32_t> m_ImageHorIter, m_ImageVerIter;

    const Scene* m_ActiveScene = nullptr;
    const CustomCamera* m_ActiveCamera = nullptr;

    int m_ScreenWidth;
    int m_ScreenHeight;
};
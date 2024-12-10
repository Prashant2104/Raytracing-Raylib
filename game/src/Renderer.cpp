#include "Renderer.h"
#include "raymath.h"

namespace Utils {

    static Vector4 Vector4Clamp(const Vector4& vector, Vector4 min, Vector4 max) {
        Vector4 result = { 0.0f };

        result.x = fminf(max.x, fmaxf(min.x, vector.x));
        result.y = fminf(max.y, fmaxf(min.y, vector.y));
        result.z = fminf(max.z, fmaxf(min.z, vector.z));
        result.w = fminf(max.w, fmaxf(min.w, vector.w));

        return result;
    }
}

Renderer::Renderer(const Scene& scene, const CustomCamera& camera):
    m_ActiveScene(&scene), m_ActiveCamera(&camera)
{
    OnResize();
}

void Renderer::OnResize()
{
    m_ScreenWidth = GetScreenWidth();
    m_ScreenHeight = GetScreenHeight();
    if (IsImageValid(m_FinalImage))
    {
        ImageResize(&m_FinalImage, m_ScreenWidth, m_ScreenHeight);
    }
    else
    {
        m_FinalImage = GenImageColor(m_ScreenWidth, m_ScreenHeight, RAYWHITE);
    }
    UnloadTexture(m_Texture2D);
    m_Texture2D = LoadTextureFromImage(m_FinalImage);
}

void Renderer::OnSphereMove()
{
    m_InitialRenderPass = false;
}

void Renderer::UpdateTextureBuffer()
{
    if (!m_ActiveCamera->IsCameraMoving() && m_InitialRenderPass) return;

    for (uint32_t y = 0; y < m_ScreenHeight; y++)
    {
        for (uint32_t x = 0; x < m_ScreenWidth; x++)
        {
            Vector4 color = PerPixel(x, y);
            color = Utils::Vector4Clamp(color, Vector4Zero(), Vector4One());
            ImageDrawPixel(&m_FinalImage, x, y, ColorFromNormalized(color));
        }
    }
    ImageFlipVertical(&m_FinalImage);
    UpdateTexture(m_Texture2D, m_FinalImage.data);
    m_InitialRenderPass = true;
}

void Renderer::Render()
{
    UpdateTextureBuffer();
    DrawTexture(m_Texture2D, 0, 0, WHITE);
}

Vector4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
    Ray ray{};
    ray.position = m_ActiveCamera->GetPosition();
    ray.direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage.width];
    
    Vector4 color{ 0.0f };
    float multiplier = 1.0f;

    int bounces = 2;
    for (int i = 0; i < bounces; i++) {
        Renderer::HitPayload payload = TraceRay(ray);

        if (payload.HitDistance < 0.0f) {
            Vector4 skyColor{ 0.0f };
            color += skyColor * multiplier;
            break;
        }

        const Sphere& closestSphere = m_ActiveScene->Spheres[payload.ObjectIndex];

        Vector3 lightDir = Vector3Normalize({ -1.0f, -1.0f, -1.0f });
        float dot = Vector3DotProduct(payload.WorldNormal, Vector3Negate(lightDir));
        float intensity = fmax(dot, 0.0f);

        Vector4 sphereColor = closestSphere.Albedo * intensity;
        color += sphereColor * multiplier;
        multiplier *= 0.7f;

        ray.position = payload.WorldPosition + payload.WorldNormal * 0.0001f;
        ray.direction = Vector3Reflect(ray.direction, payload.WorldNormal);
    }
    return Vector4{ color.x, color.y, color.z, 1.0f };
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray) const
{
    //(bx^2 + by^2) * t^2 + (2(ax * bx + ay * by)) * t + (ax^2 + ay^2 - r^2) = 0
    //a = ray origin
    //b = ray direction
    //r = radius
    //t = hit distance
    int closestSphere = -1;
    float hitDist = FLT_MAX;

    for (int i = 0; i < m_ActiveScene->Spheres.size(); i++)
    {
        const Sphere& sphere = m_ActiveScene->Spheres[i];
        Vector3 origin = ray.position - sphere.Position;

        float a = Vector3DotProduct(ray.direction, ray.direction);
        float b = 2.0f * Vector3DotProduct(origin, ray.direction);
        float c = Vector3DotProduct(origin, origin) - (sphere.Radius * sphere.Radius);

        //Quadratic formula
        //d = b^2 - 4 * a * c
        float d = b * b - 4.0f * a * c;

        if (d < 0.0f)
            continue;
        //(-b +- sqrt(d)) / (2.0f * a)
        float t[] =
        {
            (-b - sqrtf(d)) / (2.0f * a),
            (-b + sqrtf(d)) / (2.0f * a)
        };
        if (t[0] > 0.0f && t[0] < hitDist) {
            hitDist = t[0];
            closestSphere = i;
        }
    }
    if(closestSphere < 0)
        return Miss(ray);

    return ClosestHit(ray, hitDist, closestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex) const
{
    Renderer::HitPayload payload{};
    payload.HitDistance = hitDistance;
    payload.ObjectIndex = objectIndex;

    const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];

    Vector3 origin = ray.position - closestSphere.Position;
    payload.WorldPosition = origin + ray.direction * hitDistance;
    payload.WorldNormal = Vector3Normalize(payload.WorldPosition);
    payload.WorldPosition += closestSphere.Position;

    return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray) const
{
    Renderer::HitPayload payload{};
    payload.HitDistance = -1.0f;

    return payload;
}

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

Renderer::Renderer()
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

void Renderer::UpdateTextureBuffer(const Scene& scene, const CustomCamera& camera)
{
    if (!camera.IsCameraMoving() && m_InitialRenderPass) return;

    Ray ray{};
    ray.position = camera.GetPosition();
    for (int y = 0; y < m_ScreenHeight; y++)
    {
        for (int x = 0; x < m_ScreenWidth; x++)
        {
            ray.direction = camera.GetRayDirections()[x + y * m_FinalImage.width];

            Vector4 color = TraceRay(scene, ray);
            color = Utils::Vector4Clamp(color, Vector4Zero(), Vector4One());
            ImageDrawPixel(&m_FinalImage, x, y, ColorFromNormalized(color));
        }
    }
    ImageFlipVertical(&m_FinalImage);
    UpdateTexture(m_Texture2D, m_FinalImage.data);
    m_InitialRenderPass = true;
}

void Renderer::Render(const Scene& scene, const CustomCamera& camera)
{
    UpdateTextureBuffer(scene, camera);
    DrawTexture(m_Texture2D, 0, 0, WHITE);
}

Vector4 Renderer::TraceRay(const Scene& scene, const Ray& ray) const
{
    if(scene.Spheres.size() == 0)  return Vector4{ 0.9f, 0.9f, 0.9f, 1.0f };

    //(bx^2 + by^2) * t^2 + (2(ax * bx + ay * by)) * t + (ax^2 + ay^2 - r^2) = 0
    //a = ray origin
    //b = ray direction
    //r = radius
    //t = hit distance
    const Sphere* closestSphere = nullptr;
    float hitDist = FLT_MAX;
    for (const Sphere& sphere : scene.Spheres) 
    {
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
        if (t[0] < hitDist) {
            hitDist = t[0];
            closestSphere = &sphere;
        }
    }
    if(closestSphere == nullptr)
        return Vector4{ 0.9f, 0.9f, 0.9f, 1.0f };

    Vector3 origin = ray.position - closestSphere->Position;
    Vector3 hitPos = origin + ray.direction * hitDist;
    Vector3 normal = Vector3Normalize(hitPos);

    Vector3 lightDir = Vector3Normalize({ -1.0f, -1.0f, -1.0f });
    float dot = Vector3DotProduct(normal, Vector3Negate(lightDir));
    float intensity = fmax(dot, 0.0f);

    Vector4 sphereColor = closestSphere->Albedo * intensity;

    return Vector4{ sphereColor.x, sphereColor.y, sphereColor.z, sphereColor.w };
}
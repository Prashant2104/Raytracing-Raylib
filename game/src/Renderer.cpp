#include "Renderer.h"
#include "raymath.h"
#include "Random.h"
#include <execution>

#define MT 1;

namespace Utils {
    static Vector4 Vector4Clamp(const Vector4& vector, Vector4 min, Vector4 max) {
        Vector4 result = { 0.0f };

        result.x = fminf(max.x, fmaxf(min.x, vector.x));
        result.y = fminf(max.y, fmaxf(min.y, vector.y));
        result.z = fminf(max.z, fmaxf(min.z, vector.z));
        result.w = fminf(max.w, fmaxf(min.w, vector.w));

        return result;
    }
    static Vector4 multiplyVector4ByVector3(const Vector4& v4, const Vector3& v3) {
        // Multiply the first 3 components of Vector4 with Vector3
        return Vector4{ v4.x * v3.x, v4.y * v3.y, v4.z * v3.z, v4.w };
    }
    static Vector3 multiplyVector3ByVector4(const Vector3& v3, const Vector4& v4) {
        // Multiply the first 3 components of Vector4 with Vector3
        return Vector3{ v4.x * v3.x, v4.y * v3.y, v4.z * v3.z };
    }
}

Renderer::Renderer(const Scene& scene, const CustomCamera& camera):
    m_ActiveScene(&scene), m_ActiveCamera(&camera)
{
    m_ScreenWidth = GetScreenWidth();
    m_ScreenHeight = GetScreenHeight();

    m_FinalImage = GenImageColor(m_ScreenWidth, m_ScreenHeight, RAYWHITE);
    m_Texture2D = LoadTextureFromImage(m_FinalImage);

    OnResize();
}

void Renderer::OnResize()
{
    m_ScreenWidth = GetScreenWidth();
    m_ScreenHeight = GetScreenHeight();

    delete[] m_AccumulationData;
    m_AccumulationData = new Vector4[m_ScreenWidth * m_ScreenHeight];

    UnloadImage(m_FinalImage);
    m_FinalImage = GenImageColor(m_ScreenWidth, m_ScreenHeight, RAYWHITE);

    UnloadTexture(m_Texture2D);
    m_Texture2D = LoadTextureFromImage(m_FinalImage);

    m_ImageHorIter.resize(m_ScreenWidth);
    m_ImageVerIter.resize(m_ScreenHeight);
    for (uint32_t i = 0; i < m_ScreenWidth; i++)
        m_ImageHorIter[i] = i;
    for (uint32_t i = 0; i < m_ScreenHeight; i++)
        m_ImageVerIter[i] = i;

    ResetFrameIndex();
}

void Renderer::ExportRender(const char* name) const
{
    char* directory = "./Renders/";
    if (!DirectoryExists(directory)) {
        MakeDirectory(directory);
    }
    char t[256];
    strcpy(t, directory);
    strcat(t, name);
    Image renderImage = ImageCopy(m_FinalImage);
    //ImageResizeNN(&renderImage, renderImage.width * 2.0f, renderImage.height * 2.0f);
    ExportImage(renderImage, t);
    UnloadImage(renderImage);
}

void Renderer::UpdateTextureBuffer()
{
    if (m_FrameIndex == 1)
        memset(m_AccumulationData, 0, m_ScreenWidth * m_ScreenHeight * sizeof(Vector4));
#if MT
    std::for_each(std::execution::par, m_ImageVerIter.begin(), m_ImageVerIter.end(), [this](uint32_t y)
        {
            std::for_each(std::execution::par, m_ImageHorIter.begin(), m_ImageHorIter.end(), [this, y](uint32_t x)
                {
                    Vector4 color = PerPixel(x, y);
                    m_AccumulationData[x + y * m_ScreenWidth] += color;

                    Vector4 accumulatedColor = m_AccumulationData[x + y * m_ScreenWidth];
                    Vector4 frameVec4 = { (float)m_FrameIndex,(float)m_FrameIndex ,(float)m_FrameIndex ,(float)m_FrameIndex };
                    accumulatedColor = Vector4Divide(accumulatedColor, frameVec4);

                    accumulatedColor = Utils::Vector4Clamp(accumulatedColor, Vector4Zero(), Vector4One());
                    ImageDrawPixel(&m_FinalImage, x, y, ColorFromNormalized(accumulatedColor));
                });

        });
#else
    for (uint32_t y = 0; y < m_ScreenHeight; y++)
    {
        for (uint32_t x = 0; x < m_ScreenWidth; x++)
        {
            Vector4 color = PerPixel(x, y);
            m_AccumulationData[x + y * m_ScreenWidth] += color;
            
            Vector4 accumulatedColor = m_AccumulationData[x + y * m_ScreenWidth];
            Vector4 frameVec4 = { (float)m_FrameIndex,(float)m_FrameIndex ,(float)m_FrameIndex ,(float)m_FrameIndex };
            accumulatedColor = Vector4Divide(accumulatedColor, frameVec4);

            accumulatedColor = Utils::Vector4Clamp(accumulatedColor, Vector4Zero(), Vector4One());
            ImageDrawPixel(&m_FinalImage, x, y, ColorFromNormalized(accumulatedColor));
        }
    }
#endif

    ImageFlipVertical(&m_FinalImage);
    UpdateTexture(m_Texture2D, m_FinalImage.data);

    if (m_Settings.accumulate)
        m_FrameIndex++;
    else
        m_FrameIndex = 1;
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
    
    Vector4 light{ 0.0f };
    Vector3 contribution = { 1.0f, 1.0f, 1.0f };

    int bounces = 15;
    for (int i = 0; i < bounces; i++) {
        Renderer::HitPayload payload = TraceRay(ray);

        if (payload.HitDistance < 0.0f) {
            Vector4 skyColor = Vector4Ones * 0.1f;
            light += Utils::multiplyVector4ByVector3(skyColor, contribution);
            break;
        }

        const Sphere& closestSphere = m_ActiveScene->Spheres[payload.ObjectIndex];
        const Mat& mat = m_ActiveScene->Materials[closestSphere.MaterialID];

        /*Vector3 lightDir = Vector3Normalize({ -1.0f, -1.0f, -1.0f });
        float dot = Vector3DotProduct(payload.WorldNormal, Vector3Negate(lightDir));
        float intensity = fmax(dot, 0.0f);*/

        Vector4 sphereColor =  ColorNormalize(mat.Albedo);// *intensity;
        //light += Utils::multiplyVector4ByVector3(sphereColor, contribution);
        contribution = Utils::multiplyVector3ByVector4(contribution, sphereColor);
        light += mat.GetEmission();

        ray.position = payload.WorldPosition + payload.WorldNormal * 0.0001f;
        Vector3 randomVector = RayTracing::Random::Vec3(-0.5f, 0.5f);
        randomVector *= mat.Roughness;
        //ray.direction = Vector3Reflect(ray.direction, payload.WorldNormal + randomVector);
        ray.direction = Vector3Normalize(payload.WorldNormal + RayTracing::Random::InUnitSphere());
    }
    return Vector4{ light.x, light.y, light.z, 1.0f };
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray) const
{
    //(bx^2 + by^2) * t^2 + 2 * (ax * bx + ay * by) * t + ((ax^2 + ay^2) - r^2) = 0
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

        //Quadratic formula
        //ax^2 + bx + c = 0
        //a = (bx^2 + by^2)
        //b = 2 * (ax * bx + ay * by)
        //c = (ax^2 + ay^2) - r^2
        float a = Vector3LengthSqr(ray.direction);
        float b = Vector3DotProduct(origin, ray.direction);
        float c = Vector3LengthSqr(origin) - (sphere.Radius * sphere.Radius);

        //Determinent of quadratic formula
        //d = b^2 - 4 * a * c
        float d = b * b - a * c;

        if (d < 0.0f)
            continue;
        //Solving for t
        //(-b +- sqrt(d)) / (2.0f * a)
        float t[] =
        {
            (-b - sqrtf(d)) / (a),
            (-b + sqrtf(d)) / (a)
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
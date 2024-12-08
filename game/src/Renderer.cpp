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
    m_SphereColor = { 1.0f, 0.0f, 0.0f };
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
        m_FinalImage = GenImageColor(m_ScreenWidth, m_ScreenHeight, WHITE);
    }
    for (int y = 0; y < m_ScreenHeight; y++) 
    {
        for (int x = 0; x < m_ScreenWidth; x++)
        {
            Vector2 coord = { (float)x / (float)m_FinalImage.width, (float)y / (float)m_FinalImage.height };
            coord = Vector2SubtractValue((coord * 2.0f), 1.0f);  // Converting from  0 -> 1 to -1 -> 1
            coord.x *= ((float)m_ScreenWidth / (float)m_ScreenHeight); //compensating for the aspect ratio

            Vector4 color = TraceRay(coord);
            color = Utils::Vector4Clamp(color, Vector4Zero(), Vector4One());
            ImageDrawPixel(&m_FinalImage, x, y, ColorFromNormalized(color));
            //ImageDrawPixel(&m_FinalImage, x, y, GetColor(RayTracing::Random::UInt(0x000000ff, 0xffffffff)));
        }
    }
    ImageFlipVertical(&m_FinalImage);
    UnloadTexture(m_Texture2D);
    m_Texture2D = LoadTextureFromImage(m_FinalImage);
}

void Renderer::Render() const
{
    DrawTexture(m_Texture2D, 0, 0, WHITE);
}

void Renderer::ChangeColor()
{
    m_ColorIndex = (m_ColorIndex+1) % 5;
    switch (m_ColorIndex)
    {
    case 0:
        m_SphereColor = { 1.0f, 0.0f, 0.0f };
        break;
    case 1:
        m_SphereColor = { 0.0f, 1.0f, 0.0f };
        break;
    case 2:
        m_SphereColor = { 0.0f, 0.0f, 1.0f };
        break;
    case 3:
        m_SphereColor = { 1.0f, 1.0f, 1.0f };
        break;
    case 4:
        m_SphereColor = { 1.0f, 0.0f, 1.0f };
        break;
    default:
        m_SphereColor = { 1.0f, 0.0f, 1.0f };
        break;
    }
    OnResize();
}

Vector4 Renderer::TraceRay(Vector2 coord)
{   
    Vector3 rayOrigin = { 0.0f, 0.0f, 1.0f };
    Vector3 rayDirection = { coord.x, coord.y, -1.0f };
    //rayDirection = Vector3Normalize(rayDirection);
    float radius = 0.5f;

    //(bx^2 + by^2) * t^2 + (2(ax * bx + ay * by)) * t + (ax^2 + ay^2 - r^2) = 0
    //a = ray origin
    //b = ray direction
    //r = radius
    //t = hit distance
    float a = Vector3DotProduct(rayDirection, rayDirection);
    float b = 2.0f * Vector3DotProduct(rayOrigin, rayDirection);
    float c = Vector3DotProduct(rayOrigin, rayOrigin) - (radius * radius);

    //Quadratic formula
    //d = b^2 - 4 * a * c
    float d = b * b - 4.0f * a * c;

    if (d < 0.0f) 
        return Vector4{ 0.0f, 0.0f, 0.0f, 1.0f };

    //(-b +- sqrt(d)) / (2.0f * a)
    float t[] =
    {
        (-b - sqrtf(d)) / (2.0f * a),
        (-b + sqrtf(d)) / (2.0f * a)
    };

    Vector3 hitPos = rayOrigin + rayDirection * t[0];
    Vector3 normal = Vector3Normalize(hitPos);

    Vector3 lightDir = Vector3Normalize({ -1.0f, -1.0f, -1.0f });
    float dot = Vector3DotProduct(normal, Vector3Negate(lightDir));
    float intensity = fmax(dot, 0.0f);

    Vector3 sphereColor = m_SphereColor * intensity;

    return Vector4 { sphereColor.x, sphereColor.y, sphereColor.z, 1.0f };
}
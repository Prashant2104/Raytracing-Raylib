#include "CustomCamera.h"

namespace Utils {
    static Quaternion QuaternionCross(const Quaternion& q1, const Quaternion& q2) {
        Quaternion result{
            q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,  // w
            q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,  // x
            q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,  // y
            q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w   // z
        };
        return result;
    }
}

CustomCamera::CustomCamera(float verticalFOV, float nearClip, float farClip)
    : m_VerticalFOV(verticalFOV), m_NearClip(nearClip), m_FarClip(farClip)
{
    m_ForwardDirection = { 0, 0, -1 };
    m_Position = { 0, 0, 3 };
}

void CustomCamera::OnUpdate(float ts)
{
    Vector2 mousePos = GetMousePosition();
    Vector2 delta = (mousePos - m_LastMousePosition) * 0.002f;
    m_LastMousePosition = mousePos;

    if (!IsMouseButtonDown(0))
    {
        EnableCursor();
        return;
    }
    DisableCursor();

    bool moved = false;

    Vector3 rightDirection = Vector3CrossProduct(m_ForwardDirection, Vector3UnitY);

    float speed = 5.0f;

    // Movement
    int keyPressed = GetKeyPressed();
    switch (keyPressed)
    {
    case KEY_LEFT:
    case KEY_A:
        m_Position -= rightDirection * speed * ts;
        moved = true;
        break;

    case KEY_RIGHT:
    case KEY_D:
        m_Position += rightDirection * speed * ts;
        moved = true;
        break;

    case KEY_DOWN:
    case KEY_S:
        m_Position -= m_ForwardDirection * speed * ts;
        moved = true;
        break;

    case KEY_UP:
    case KEY_W:
        m_Position += m_ForwardDirection * speed * ts;
        moved = true;
        break;

    case KEY_Q:
        m_Position -= Vector3UnitY * speed * ts;
        moved = true;
        break;

    case KEY_E:
        m_Position += Vector3UnitY * speed * ts;
        moved = true;
        break;

    default:
        break;
    }

    // Rotation
    if (delta.x != 0.0f || delta.y != 0.0f)
    {
        float pitchDelta = delta.y * GetRotationSpeed();
        float yawDelta = delta.x * GetRotationSpeed();

        Quaternion q = QuaternionNormalize(Utils::QuaternionCross(
            QuaternionFromAxisAngle(rightDirection, -pitchDelta),
            QuaternionFromAxisAngle(Vector3UnitY, -yawDelta)));
        m_ForwardDirection = Vector3RotateByQuaternion(m_ForwardDirection, q);

        moved = true;
    }

    if (moved)
    {
        RecalculateView();
        RecalculateRayDirections();
    }
}

void CustomCamera::OnResize()
{
    m_ViewportWidth = GetScreenWidth();
    m_ViewportHeight = GetScreenHeight();

    RecalculateProjection();
    RecalculateRayDirections();
}

float CustomCamera::GetRotationSpeed()
{
    return 0.3f;
}

void CustomCamera::RecalculateProjection()
{
    m_Projection = MatrixPerspective((DEG2RAD * m_VerticalFOV), (float)m_ViewportWidth / (float)m_ViewportHeight, m_NearClip, m_FarClip);
    m_InverseProjection = MatrixInvert(m_Projection);
}

void CustomCamera::RecalculateView()
{
    m_View = MatrixLookAt(m_Position, m_Position + m_ForwardDirection, Vector3UnitY);
    m_InverseView = MatrixInvert(m_View);
}

void CustomCamera::RecalculateRayDirections()
{
    m_RayDirections.resize(m_ViewportWidth * m_ViewportHeight);

    for (uint32_t y = 0; y < m_ViewportHeight; y++)
    {
        for (uint32_t x = 0; x < m_ViewportWidth; x++)
        {
            Vector2 coord = { (float)x / (float)m_ViewportWidth, (float)y / (float)m_ViewportHeight };
            coord = Vector2SubtractValue((coord * 2.0f), 1.0f);  // Converting from  0 -> 1 to -1 -> 1
            Vector4 target = QuaternionTransform(Vector4{ coord.x, coord.y, 1, 1 }, m_InverseProjection);

            Vector3 normalizesTarget = Vector3{ target.x, target.y, target.z } / target.w;
            normalizesTarget = Vector3Normalize(normalizesTarget);

            Quaternion inverseTarget = QuaternionTransform(Vector4{ normalizesTarget.x, normalizesTarget.y, normalizesTarget.z, 0 }, m_InverseView);

            Vector3 rayDirection = { inverseTarget.x, inverseTarget.y, inverseTarget.z };
            m_RayDirections[x + y * m_ViewportWidth] = rayDirection;
        }
    }
}
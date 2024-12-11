#include "CustomCamera.h"

namespace Utils {
    static Quaternion QuaternionCross(const Quaternion& q1, const Quaternion& q2) {
        Quaternion result{
            q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,  // x
            q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,  // y
            q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,  // z
            q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z   // w
        };
        return result;
    }
}

CustomCamera::CustomCamera(float verticalFOV, float nearClip, float farClip)
    : m_VerticalFOV(verticalFOV), m_NearClip(nearClip), m_FarClip(farClip)
{
    m_ForwardDirection = { 0.0f, 0.0f, -1.0f };
    m_Position = { 0.0f, 0.0f, 5.0f };
    
    RecalculateView();
    RecalculateRayDirections();
    OnResize();
}

void CustomCamera::OnUpdate(float ts)
{
    Vector2 delta = GetMouseDelta() * 0.003;

    if (!IsMouseButtonDown(1))
    {
        if(IsCursorHidden())
            EnableCursor();
        return;
    }
    if (!IsCursorHidden())
        DisableCursor();

    m_IsMoving = false;

    Vector3 rightDirection = Vector3CrossProduct(m_ForwardDirection, Vector3UnitY);

    float speed = 1.0f;

    // Movement
    if (IsKeyDown(KEY_W))
    {
        m_Position += m_ForwardDirection * speed * ts;
        m_IsMoving = true;
    }
    else if (IsKeyDown(KEY_S))
    {
        m_Position -= m_ForwardDirection * speed * ts;
        m_IsMoving = true;
    }
    if (IsKeyDown(KEY_A))
    {
        m_Position -= rightDirection * speed * ts;
        m_IsMoving = true;
    }
    else if (IsKeyDown(KEY_D))
    {
        m_Position += rightDirection * speed * ts;
        m_IsMoving = true;
    }
    if (IsKeyDown(KEY_Q))
    {
        m_Position -= Vector3UnitY * speed * ts;
        m_IsMoving = true;
    }
    else if (IsKeyDown(KEY_E))
    {
        m_Position += Vector3UnitY * speed * ts;
        m_IsMoving = true;
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

        m_IsMoving = true;
    }

    if (m_IsMoving)
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
    return 0.03f;
}

bool CustomCamera::IsCameraMoving() const
{
    return m_IsMoving;
}

void CustomCamera::RecalculateProjection()
{
    m_Projection = MatrixPerspective((0.01745329251994329576923690768489 * m_VerticalFOV), (float)m_ViewportWidth / (float)m_ViewportHeight, m_NearClip, m_FarClip);
    m_InverseProjection = MatrixInvert(m_Projection);
}

void CustomCamera::RecalculateView()
{
    //UpdateCameraPro(&m_Camera, m_Position, m_ForwardDirection, 1);
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

            Vector4 target = Vector4{ coord.x, coord.y, 1, 1 } * m_InverseProjection;

            Vector3 normalizesTarget = Vector3Normalize(Vector3{ target.x, target.y, target.z } / target.w);

            Quaternion inverseTarget = Vector4{ normalizesTarget.x, normalizesTarget.y, normalizesTarget.z, 0 } * m_InverseView;
            Vector3 rayDirection = { inverseTarget.x, inverseTarget.y, inverseTarget.z };

            //Vector3 rayDirection = { coord.x, coord.y, -1.0f };
            m_RayDirections[x + y * m_ViewportWidth] = rayDirection;
        }
    }
}
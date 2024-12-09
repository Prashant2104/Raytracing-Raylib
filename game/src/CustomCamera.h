#pragma once
#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <vector>

class CustomCamera
{
public:
    CustomCamera(float verticalFOV, float nearClip, float farClip);

    void OnUpdate(float ts);
    void OnResize();

    const Matrix& GetProjection() const { return m_Projection; }
    const Matrix& GetInverseProjection() const { return m_InverseProjection; }
    const Matrix& GetView() const { return m_View; }
    const Matrix& GetInverseView() const { return m_InverseView; }

    const Vector3& GetPosition() const { return m_Position; }
    const Vector3& GetDirection() const { return m_ForwardDirection; }

    const std::vector<Vector3>& GetRayDirections() const { return m_RayDirections; }

    float GetRotationSpeed();
    bool IsCameraMoving() const;
private:
    void RecalculateProjection();
    void RecalculateView();
    void RecalculateRayDirections();
private:
    Matrix m_Projection{ 1.0f };
    Matrix m_View{ 1.0f };
    Matrix m_InverseProjection{ 1.0f };
    Matrix m_InverseView{ 1.0f };

    float m_VerticalFOV = 45.0f;
    float m_NearClip = 0.1f;
    float m_FarClip = 100.0f;

    Vector3 m_Position{ 0.0f, 0.0f, 0.0f };
    Vector3 m_ForwardDirection{ 0.0f, 0.0f, 0.0f };

    // Cached ray directions
    std::vector<Vector3> m_RayDirections;

    Vector2 m_LastMousePosition{ 0.0f, 0.0f };

    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

    bool m_IsMoving = false;
};

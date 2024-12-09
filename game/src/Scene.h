#pragma once
#include "raylib.h"
#include <vector>
struct Sphere {
    Vector3 Position{ 0.0f, 0.0f, 0.0f };
    float Radius = 0.5f;
    Vector3 Albedo{ 1.0f, 1.0f, 1.0f };
};
struct Scene {
    std::vector<Sphere> Spheres;
};
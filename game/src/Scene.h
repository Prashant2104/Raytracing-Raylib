#pragma once
#include "raylib.h"
#include <vector>
struct Mat {
    Color Albedo = ColorFromNormalized({ 1.0f, 1.0f, 1.0f, 1.0f });
    float Roughness = 0.10f;
    float Metallic = 0.0f;
};
struct Sphere {
    Vector3 Position{ 0.0f, 0.0f, 0.0f };
    float Radius = 0.5f;
    int MaterialID = 0;
};
struct Scene {
    std::vector<Sphere> Spheres;
    std::vector<Mat> Materials;
};
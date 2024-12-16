#pragma once
#include "raylib.h"
#include <vector>
struct Mat {
    Color Albedo = WHITE;
    float Roughness = 0.10f;
    float Metallic = 0.0f;
    float EmissionPower = 0.0f;
    Color EmissionColor = BLACK;
    Vector4 GetEmission() const { return ColorNormalize(EmissionColor) * EmissionPower; }
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
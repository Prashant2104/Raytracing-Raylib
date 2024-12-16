#pragma once
#include <random>
#include <limits>
#include "raylib.h"

namespace RayTracing {

    class Random
    {
    public:
        static uint32_t UInt()
        {
            return GetRandomEngine().s_Distribution(GetRandomEngine().s_RandomEngine);
        }

        static uint32_t UInt(uint32_t min, uint32_t max)
        {
            return min + (GetRandomEngine().s_Distribution(GetRandomEngine().s_RandomEngine) % (max - min + 1));
        }

        static float Float()
        {
            return (float)GetRandomEngine().s_Distribution(GetRandomEngine().s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
        }

        static Vector3 Vec3(float min, float max)
        {
            return Vector3{ Float() * (max - min) + min, Float() * (max - min) + min, Float() * (max - min) + min };
        }

        static Vector3 InUnitSphere() 
        {
            return Vector3Normalize(Vec3(-1.0f, 1.0f));
        }

    private:
        // Using thread-local storage for thread-specific random engines
        struct ThreadLocalEngine {
            std::mt19937 s_RandomEngine;
            std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;

            ThreadLocalEngine()
                : s_RandomEngine(std::random_device()()), // seed using a random device
                s_Distribution(0, std::numeric_limits<uint32_t>::max()) // uniform distribution for uint32_t
            {}
        };

        // Function to get the thread-local engine
        static ThreadLocalEngine& GetRandomEngine()
        {
            thread_local ThreadLocalEngine engine;
            return engine;
        }
    };
}

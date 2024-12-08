#pragma once
#include <random>

namespace RayTracing {

    class Random
    {
    public:
        static void Init()
        {
            s_RandomEngine.seed(std::random_device()());
        }

        static uint32_t UInt()
        {
            return s_Distribution(s_RandomEngine);
        }

        static uint32_t UInt(uint32_t min, uint32_t max)
        {
            return min + (s_Distribution(s_RandomEngine) % (max - min + 1));
        }

        static float Float()
        {
            return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
        }

    private:
        static std::mt19937 s_RandomEngine;
        static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
    };

}
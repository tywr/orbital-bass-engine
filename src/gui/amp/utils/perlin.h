#pragma once
#include <algorithm>
#include <numeric>
#include <random>
#include <vector>
#include <cmath> // Required for std::floor

class PerlinNoise
{
  public:
    PerlinNoise()
    {
        reseed(2025);
    }

    void reseed(unsigned int seed)
    {
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);
        std::default_random_engine engine(seed);
        std::shuffle(p.begin(), p.end(), engine);
        p.insert(p.end(), p.begin(), p.end());
    }

    double noise(double x, double y, double z) const
    {
        const int X = static_cast<int>(std::floor(x)) & 255;
        const int Y = static_cast<int>(std::floor(y)) & 255;
        const int Z = static_cast<int>(std::floor(z)) & 255;

        x -= std::floor(x);
        y -= std::floor(y);
        z -= std::floor(z);

        const double u = fade(x);
        const double v = fade(y);
        const double w = fade(z);

        // FINAL FIX: Explicitly cast the 'int' index to 'size_t' right before vector access.
        const int A = p[static_cast<size_t>(X)] + Y;
        const int AA = p[static_cast<size_t>(A)] + Z;
        const int AB = p[static_cast<size_t>(A + 1)] + Z;

        const int B = p[static_cast<size_t>(X + 1)] + Y;
        const int BA = p[static_cast<size_t>(B)] + Z;
        const int BB = p[static_cast<size_t>(B + 1)] + Z;

        return lerp(
            w,
            lerp(
                v,
                // FINAL FIX: Cast all indices here as well.
                lerp(u, grad(p[static_cast<size_t>(AA)], x, y, z), grad(p[static_cast<size_t>(BA)], x - 1.0, y, z)),
                lerp(u, grad(p[static_cast<size_t>(AB)], x, y - 1.0, z), grad(p[static_cast<size_t>(BB)], x - 1.0, y - 1.0, z))
            ),
            lerp(
                v,
                lerp(u, grad(p[static_cast<size_t>(AA + 1)], x, y, z - 1.0), grad(p[static_cast<size_t>(BA + 1)], x - 1.0, y, z - 1.0)),
                lerp(u, grad(p[static_cast<size_t>(AB + 1)], x, y - 1.0, z - 1.0), grad(p[static_cast<size_t>(BB + 1)], x - 1.0, y - 1.0, z - 1.0))
            )
        );
    }

  private:
    static double fade(double t)
    {
        return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
    }

    static double lerp(double t, double a, double b)
    {
        return a + t * (b - a);
    }

    static double grad(int hash, double x, double y, double z)
    {
        const int h = hash & 15;
        const double u = h < 8 ? x : y;
        const double v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    std::vector<int> p;
};

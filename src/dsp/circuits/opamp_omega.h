#pragma once
#include "../maths/omega.h"
#include <cmath>

class OpAmp
{
  public:
    OpAmp()
    {
        // Calculate v0 at initialization
        v0 = vt * omega((bias + 1.0f) / vt) - 2.0f;
        v1 = knee(x1);
        // Numerical derivative of knee at x1
        v1p = (knee(x1 + eps) - knee(x1 - eps)) / (2 * eps);
        // Calculate coefficients for cubic polynomial approximation
        a = (v1p * x1 - v1) / (2 * x1 * x1 * x1);
        b = (-v1p * x1 + 3 * v1) / (2 * x1);
    }
    float processSample(float);
    float omega(float);
    float knee(float);

  private:
    // Fixed variables
    float bias = 1.02f;
    float vt = 0.0005f;
    float eps = 1e-4f;
    float k = 100;
    float x1 = bias - k * vt;
    float v0;
    float v1;
    float v1p;
    float a;
    float b;
};

inline float OpAmp::omega(float x)
{
    if (std::abs(x) > 1.5f)
    {
        return omega4(x);
    }
    else
    {
        float c0 = 0.5671432904097838f;
        float c1 = 0.3618963236098023f;
        float c2 = 0.0736778463779836f;
        float c3 = -0.0013437346889135f;
        float c4 = -0.0016355437889344f;
        float c5 = 0.0002166542734346f;

        return c0 + c1 * x + c2 * std::pow(x, 2.0f) + c3 * std::pow(x, 3.0f) +
               c4 * std::pow(x, 4.0f) + c5 * std::pow(x, 5.0f);
    }
}

inline float OpAmp::knee(float x)
{
    float expx = std::exp(-x / vt);
    float in_omega = (-x + bias + expx) / vt;
    float vout = vt * omega(in_omega) - expx;
    return -(vout - 1.0f - v0) / (1.0f + v0);
}

inline float OpAmp::processSample(float x)
{
    float x_abs = std::abs(x);
    float y;
    if (x_abs >= x1)
    {
        y = knee(x_abs);
    }
    else
    {
        y = a * std::pow(x_abs, 3.0f) + b * x_abs;
    }

    if (x < 0.0f)
        y = -y;
    return y;
}

// inline float OpAmp::processSample(float x)
// {
//     return x / std::pow(1.0f + std::pow(x, 100.0f), 1.0f / 100.0f);
// }

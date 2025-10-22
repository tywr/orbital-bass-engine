#pragma once
#include "../maths/omega.h"
#include <cmath>

class OpAmp
{
  public:
    OpAmp()
    {
        C = 0.5f * vt * std::log(std::cosh(1.0f / vt));
    }
    float processSample(float);
    float omega(float);
    float knee(float);

  private:
    // Fixed variables
    float vt = 0.026f;
    float C;
};

inline float OpAmp::processSample(float x)
{
    // For large input, don't bother calculating the non-linearity
    if (x > 2.0f)
    {
        return 1.0f;
    }
    else if (x < -2.0f)
    {
        return -1.0f;
    }

    // Soft clipping using hyperbolic tangent
    if (x > 0.0f)
    {
        return 0.5f * x - 0.5f * vt * std::log(std::cosh((x - 1.0f) / vt)) + C;
    }
    else
    {
        return 0.5f * x + 0.5f * vt * std::log(std::cosh((-x - 1.0f) / vt)) - C;
    }
}

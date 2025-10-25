#pragma once

#include "../maths/omega.h"
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <juce_dsp/juce_dsp.h>
#include <utility>

class CMOS2
{
  public:
    CMOS2()
    {
    }
    void reset()
    {
    }

    float omega(float);
    float processSample(float);

  private:
    // static constexpr float s1_p = 0.04f;
    // static constexpr float s2_p = 0.25f;
    // static constexpr float s1_m = 0.2f;
    // static constexpr float s2_m = 0.4f;

    static constexpr float s1_p = 0.04f;
    static constexpr float s2_p = 0.25f;
    static constexpr float s1_m = 0.5f;
    static constexpr float s2_m = 0.1f;
};

inline float CMOS2::omega(float x)
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

inline float CMOS2::processSample(float x)
{
    float sign = (x >= 0.0f) ? 1.0f : -1.0f;
    float s1 = (sign > 0.0f) ? s1_p : s1_m;
    float s2 = (sign > 0.0f) ? s2_p : s2_m;
    return sign *
           (1.0f - 2.0f / (1.0f + sign * x / s1 + omega(1.0f + sign * x / s2)));
}

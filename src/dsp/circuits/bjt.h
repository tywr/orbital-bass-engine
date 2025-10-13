#pragma once
#include "../maths/omega.h"
#include <cmath>

class BJT
{
  public:
    BJT() {}
    float processSample(float);
    float omega(float);

  private:
    // Fixed variables
    float vp = 9.0f;
    float vt = 0.026f;
    float i_s = 0.1e-15f;
    float beta_f = 100.0f;
    float re = 1000.0f;
    float k = std::log((i_s * re / vt) * (1 + 1 / beta_f));
};

inline float BJT::omega(float x)
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

inline float BJT::processSample(float s)
{
    float vref = vp / 2;
    float vin = s + vref;
    if (vin >= vp)
    {
        float v_x = i_s * re * (1 + 1 / beta_f);
        float v_out = vt * omega(((vp + v_x) / vt) + k) - v_x;
        return v_out - vref;
    }
    float v_x = i_s * re * (std::exp((vin - vp) / vt) + 1 / beta_f);
    float v_out = vt * omega(((vin + v_x) / vt) + k) - v_x;
    return v_out - vref;
}

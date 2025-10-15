#pragma once
#include <algorithm>
#include <cmath>
#include <tuple>

class CMOSPoly
// this is a 6th order polynomial approximation of the CMOS inverter
// transfer function, taking input centered around 0V
{
  public:
    CMOSPoly()
    {
    }
    float processSample(float);

  private:
    float v0 = 0.34486360573756947f;
    float a0 = 8.22062734e+01f;
    float a1 = -7.95763591e+01;
    float a2 = 3.20445154e+01f;
    float a3 = -6.84952584e+00f;
    float a4 = 8.07789644e-01f;
    float a5 = -4.93560106e-02f;
    float a6 = 1.19407722e-03f;
};

inline float CMOSPoly::processSample(float vin)
{
    // Shift input from 0V to 4.5V bias
    float x = vin + 4.5f;
    if (x <= 3.5f)
    {
        // limit at 3.5f to avoid any overflow of exponential
        x = 3.5f;
    }
    if (x >= 9.49f)
    {
        return 0.0f;
    }
    // Horner's method for polynomial evaluation
    float y =
        ((((((a6 * x + a5) * x + a4) * x + a3) * x + a2) * x + a1) * x + a0);
    float out = (std::exp(y) - v0) / v0;
}

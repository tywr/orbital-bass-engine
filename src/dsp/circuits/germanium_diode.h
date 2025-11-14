#pragma once
#include "../maths/omega.h"
#include <cmath>
#include <juce_dsp/juce_dsp.h>

class GermaniumDiode
// Simulates a germanium diode clipper pair using the Shockley diode equation
// for more referernce about this modelization, you can read about it in
// the paper:
//
// Stefano d'Angelo, Leonardo Gabrielli & Luca Turchet
// (2019.)
// "FAST APPROXIMATION OF THE LAMBERT W FUNCTION FOR VIRTUAL ANALOG MODELLING"
// https://dafx.de/paper-archive/2019/DAFx2019_paper_5.pdf
{
  public:
    GermaniumDiode(float fs);
    float processSample(float);
    void process(const juce::dsp::ProcessContextReplacing<float>& context);
    float omega(float);
    void reset(float t_fs)
    {
        fs = t_fs;
        prev_v = 0.0f;
        prev_p = k6 * prev_v;
    }

  private:
    // Fixed variables
    float c = 1e-8f;
    float r = 30.0f;
    float i_s = 5e-4f;
    float v_t = 0.02585f;
    float v0 = 0.36694194229685273f;

    // State variables
    float prev_v;
    float prev_p;

    // Main parameters
    float fs;

    // Fixed variables for computation
    float b0;
    float b1;
    float a1;

    float crb_1;
    float k1;
    float k2;
    float k3;
    float k4;
    float k5;
    float k6;
};

inline float GermaniumDiode::omega(float x)
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

inline GermaniumDiode::GermaniumDiode(float t_fs)
{
    fs = t_fs;

    b0 = 2.0f / fs;
    b1 = -2.0f / fs;
    a1 = -1.0f;

    crb_1 = c * r + b0 + 1;
    k1 = 1 / (c * r);
    k2 = (c * r) / crb_1;
    k3 = (i_s * r) / crb_1;
    k4 = 1 / v_t;
    k5 = std::log((i_s * r) / crb_1 * v_t);
    k6 = b1 - a1 * b0;

    prev_v = 0.0f;
    prev_p = k6 * prev_v;
}

inline float GermaniumDiode::processSample(float vin)
{
    if (std::abs(vin) < 0.1f)
    {
        float p = k6 * vin - a1 * prev_p;
        prev_p = p;
        prev_v = vin;
        return vin;
    }
    float q = k1 * vin - prev_p;
    float rt = (q > 0.0f) ? 1.0f : ((q < 0.0f) ? -1.0f : 0.0f); // sign function
    float w = k2 * q + k3 * rt;
    float vout = w - v_t * rt * omega(k4 * rt * w + k5);
    float p = k6 * vout - a1 * prev_p;
    prev_p = p;
    prev_v = vout;
    return vout / v0;
}

inline void GermaniumDiode::process(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    auto& block = context.getOutputBlock();
    const size_t num_samples = block.getNumSamples();

    auto* ch = block.getChannelPointer(0);
    for (size_t i = 0; i < num_samples; ++i)
    {
        ch[i] = processSample(ch[i]);
    }
}

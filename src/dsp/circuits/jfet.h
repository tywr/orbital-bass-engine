#pragma once

#include <algorithm>
#include <cmath>
#include <juce_dsp/juce_dsp.h>

class JFET
{
  public:
    JFET(float v, float g, float as)
    {
        vp = v;
        gamma = g;
        asym = as;
        a = gamma * (1.0f - vp);
    }
    void reset()
    {
    }

    float processSample(float);

    void processBlock(juce::AudioBuffer<float>& buffer, int numSamples);
    void process(const juce::dsp::ProcessContextReplacing<float>& context);

  private:
    float vp;
    float gamma;
    float a;
    float asym;
};

inline float JFET::processSample(float x)
{
    const float c = (x < 0.0f) ? asym : 1.0f;
    const float xsat = vp + 1.0f / (2.0f * a);
    const float fsat = -1.0f / (4.0f * a) + 1.0f / (2.0f * a) + vp;
    const float sgnx = (x < 0.0f) ? -1.0f : 1.0f;
    const float absx = (x < 0.0f) ? -x : x;

    // Cutoff zone
    const float xc = (absx >= xsat) ? xsat : absx;
    const float fc =
        sgnx * (-a * c * (xc - vp) * (xc - vp) + c * (xc - vp) + vp) / fsat;

    // Linear zone
    const float fl = x / fsat;

    return (absx < vp) ? fl : fc;
}

inline void JFET::process(
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

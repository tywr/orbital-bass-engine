#pragma once

#include <algorithm>
#include <cmath>
#include <juce_dsp/juce_dsp.h>

class JFET
{
  public:
    JFET()
    {
    }
    void reset()
    {
    }

    float processSample(float);

    void processBlock(juce::AudioBuffer<float>& buffer, int numSamples);
    void process(const juce::dsp::ProcessContextReplacing<float>& context);

  private:
    static constexpr float vp = 0.5f;
    static constexpr float gamma = 0.3f;
    static constexpr float a = gamma * (1.0 - vp);
};

inline float JFET::processSample(float x)
{
    const float xsat = vp + 1.0 / (2.0 * a);
    const float fsat = -1.0 / (4.0 * a) + 1.0 / (2.0 * a) + vp;

    float x_abs = std::abs(x);
    float fx = -a * std::pow(std::max(x_abs - vp, 0.0f), 2) +
               std::max(x_abs - vp, 0.0f) + vp;
    fx /= fsat;

    return std::copysign(std::min(fx, 1.0f), x);
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

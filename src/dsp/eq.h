#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class EQ : juce::dsp::ProcessorBase
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(
        const juce::dsp::ProcessContextReplacing<float>& context
    ) override;
    void resetSmoothedValues();
    void reset() override;

    void set80Gain(float g)
    {
        float v = juce::jlimit(0.0f, 2.0f, g);
        b80_gain.setTargetValue(v);
        raw_b80_gain = v;
    }

    void set250Gain(float g)
    {
        float v = juce::jlimit(0.0f, 2.0f, g);
        b250_gain.setTargetValue(v);
        raw_b250_gain = v;
    }

    void set500Gain(float g)
    {
        float v = juce::jlimit(0.0f, 2.0f, g);
        b500_gain.setTargetValue(v);
        raw_b500_gain = v;
    }

    void set1500Gain(float g)
    {
        float v = juce::jlimit(0.0f, 2.0f, g);
        b1500_gain.setTargetValue(v);
        raw_b1500_gain = v;
    }

    void set3000Gain(float g)
    {
        float v = juce::jlimit(0.0f, 2.0f, g);
        b3000_gain.setTargetValue(v);
        raw_b3000_gain = v;
    }

    void set5000Gain(float g)
    {
        float v = juce::jlimit(0.0f, 2.0f, g);
        b5000_gain.setTargetValue(v);
        raw_b5000_gain = v;
    }

    void setLpfFrequency(float g)
    {
        float v = juce::jlimit(1000.0f, 10000.0f, g);
        lpf_frequency.setTargetValue(v);
        raw_lpf_frequency = v;
    }

    void setCoefficients();

  private:
    juce::dsp::ProcessSpec processSpec{-1, 0, 0};

    juce::dsp::IIR::Filter<float> lpf_filter_1;
    juce::dsp::IIR::Filter<float> lpf_filter_2;

    juce::dsp::IIR::Filter<float> b80_filter;
    float b80_freq = 80.0f;
    float b80_q = 0.742f;

    juce::dsp::IIR::Filter<float> b250_filter;
    float b250_freq = 250.0f;
    float b250_q = 1.05f;

    juce::dsp::IIR::Filter<float> b500_filter;
    float b500_freq = 500.0f;
    float b500_q = 1.080f;

    juce::dsp::IIR::Filter<float> b1500_filter;
    float b1500_freq = 1500.0f;
    float b1500_q = 1.080f;

    juce::dsp::IIR::Filter<float> b3000_filter;
    float b3000_freq = 3000.0f;
    float b3000_q = 1.636f;

    juce::dsp::IIR::Filter<float> b5000_filter;
    float b5000_freq = 5000.0f;
    float b5000_q = 0.349f;

    float smoothing_time = 0.05f;
    float raw_b80_gain, raw_b250_gain, raw_b500_gain, raw_b1500_gain,
        raw_b3000_gain, raw_b5000_gain, raw_lpf_frequency;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> b80_gain,
        b250_gain, b500_gain, b1500_gain, b3000_gain, b5000_gain, lpf_frequency;
};

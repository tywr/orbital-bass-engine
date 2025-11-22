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

    void setLowShelfGain(float g)
    {
        float v = juce::jlimit(0.0f, 2.0f, g);
        low_shelf_gain.setTargetValue(v);
        raw_low_shelf_gain = v;
    }

    void setLowMidFreq(float f)
    {
        float v = juce::jlimit(200.0f, 800.0f, f);
        low_mid_freq.setTargetValue(v);
        raw_low_mid_freq = v;
    }

    void setLowMidQ(float q)
    {
        float v = juce::jlimit(0.1f, 4.0f, q);
        low_mid_q.setTargetValue(v);
        raw_low_mid_q = v;
    }

    void setLowMidGain(float g)
    {
        float v = juce::jlimit(0.0f, 2.0f, g);
        low_mid_gain.setTargetValue(v);
        raw_low_mid_gain = v;
    }

    void setHighMidFreq(float f)
    {
        float v = juce::jlimit(800.0f, 2500.0f, f);
        high_mid_freq.setTargetValue(v);
        raw_high_mid_freq = v;
    }

    void setHighMidQ(float q)
    {
        float v = juce::jlimit(0.1f, 4.0f, q);
        high_mid_q.setTargetValue(v);
        raw_high_mid_q = v;
    }

    void setHighMidGain(float g)
    {
        float v = juce::jlimit(0.0f, 2.0f, g);
        high_mid_gain.setTargetValue(v);
        raw_high_mid_gain = v;
    }

    void setHighShelfGain(float g)
    {
        float v = juce::jlimit(0.0f, 2.0f, g);
        high_shelf_gain.setTargetValue(v);
        raw_high_shelf_gain = v;
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

    juce::dsp::IIR::Filter<float> low_shelf_filter;
    float low_shelf_freq = 80.0f;
    float low_shelf_q = 0.7f;

    juce::dsp::IIR::Filter<float> low_mid_filter;

    juce::dsp::IIR::Filter<float> high_mid_filter;

    juce::dsp::IIR::Filter<float> high_shelf_filter;
    float high_shelf_freq = 3200.0f;
    float high_shelf_q = 0.7f;

    float smoothing_time = 0.05f;
    float raw_low_shelf_gain = 1.0f;
    float raw_low_mid_freq = 400.0f, raw_low_mid_q = 0.7f, raw_low_mid_gain = 1.0f;
    float raw_high_mid_freq = 1500.0f, raw_high_mid_q = 0.7f, raw_high_mid_gain = 1.0f;
    float raw_high_shelf_gain = 1.0f;
    float raw_lpf_frequency = 3000.0f;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> low_shelf_gain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> low_mid_freq, low_mid_q, low_mid_gain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> high_mid_freq, high_mid_q, high_mid_gain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> high_shelf_gain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> lpf_frequency;
};

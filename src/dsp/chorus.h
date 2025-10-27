#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class Chorus : juce::dsp::ProcessorBase
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(
        const juce::dsp::ProcessContextReplacing<float>& context
    ) override;
    void reset() override;
    void resetSmoothedValues();

    void loadIR();
    void applyGain(juce::AudioBuffer<float>& buffer);
    void setMix(float newMix)
    {
        float v = juce::jlimit(0.0f, 1.0f, newMix);
        mix.setTargetValue(v);
        raw_mix = v;
    }
    void setRate(float new_rate)
    {
        float v = juce::jlimit(0.0f, 10.0f, new_rate);
        rate.setTargetValue(v);
        raw_rate = v;
    }
    void setDepth(float new_depth)
    {
        float v = juce::jlimit(0.0f, 10.0f, new_depth);
        depth.setTargetValue(v);
        raw_depth = v;
    }

  private:
    juce::dsp::ProcessSpec processSpec{44100.0, 0, 0};
    juce::dsp::Oscillator<float> lfo_left, lfo_right;
    size_t write_position_left, write_position_right;
    juce::AudioBuffer<float> delay_buffer;

    float max_delay_time = 0.05f;
    float smoothing_time = 0.05f;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mix, rate,
        depth;
    float raw_mix = 1.0f;
    float raw_rate = 1.0f;
    float raw_depth = 1.0f;
};

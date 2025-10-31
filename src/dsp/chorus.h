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
    void updateFilters();

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
        lfo_right.setFrequency(v);
        lfo_left.setFrequency(v);
    }
    void setDepth(float new_depth)
    {
        float v = juce::jlimit(0.0f, 10.0f, new_depth);
        depth.setTargetValue(v);
        raw_depth = v;
    }

  private:
    juce::dsp::ProcessSpec processSpec{44100.0, 0, 0};
    float smoothing_time = 0.05f;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mix, rate,
        depth, crossover;

    size_t write_position;
    juce::dsp::Oscillator<float> lfo_left, lfo_right;
    juce::dsp::DelayLine<
        float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd>
        delay_line;

    juce::dsp::IIR::Filter<float> pre_hpf;
    juce::dsp::IIR::Filter<float> bass_lpf;
    juce::dsp::IIR::Filter<float> pre_lpf;
    float pre_lpf_cutoff = 5000.0f;

    float base_delay_time = 7e-3f;
    float max_delay_time = 5e-2f;
    float raw_mix = 1.0f;
    float raw_rate = 1.0f;
    float raw_depth = 1.0f;
};

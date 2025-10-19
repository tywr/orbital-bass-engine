#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class AmpEQ
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void applyEQ(float& sample, float sampleRate);

    void setBassGain(float gain)
    {
        float v = juce::jlimit(0.0f, 10.0f, gain);
        bass_gain.setTargetValue(v);
        raw_bass_gain = v;
    }
    void setLowMidGain(float gain)
    {
        float v = juce::jlimit(0.0f, 10.0f, gain);
        low_mid_gain.setTargetValue(v);
        raw_low_mid_gain = v;
    }
    void setHighMidGain(float gain)
    {
        float v = juce::jlimit(0.0f, 10.0f, gain);
        high_mid_gain.setTargetValue(v);
        raw_high_mid_gain = v;
    }
    void setTrebleGain(float gain)
    {
        float v = juce::jlimit(0.0f, 10.0f, gain);
        treble_gain.setTargetValue(v);
        raw_treble_gain = v;
    }
    void setBypass(bool shouldBypass)
    {
        bypass = shouldBypass;
    }

    void setCoefficients();

  private:
    juce::dsp::ProcessSpec processSpec{-1, 0, 0};

    juce::dsp::IIR::Filter<float> bass_shelf;
    float bass_shelf_frequency = 100.0f;
    float bass_shelf_q = 0.707f;

    juce::dsp::IIR::Filter<float> low_mid_peak;
    float low_mid_peak_frequency = 500.0f;
    float low_mid_peak_q = 0.707f;

    juce::dsp::IIR::Filter<float> high_mid_peak;
    float high_mid_peak_frequency = 1500.0f;
    float high_mid_peak_q = 0.707f;

    juce::dsp::IIR::Filter<float> treble_shelf;
    float treble_shelf_frequency = 5000.0f;
    float treble_shelf_q = 0.707f;

    // GUI Parameters
    bool bypass = false;

    float smoothing_time = 0.05f;
    float raw_bass_gain, raw_low_mid_gain, raw_high_mid_gain, raw_treble_gain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> bass_gain,
        low_mid_gain, high_mid_gain, treble_gain;
};

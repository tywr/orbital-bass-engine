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
        bass_gain = gain;
    }
    void setLowMidGain(float gain)
    {
        low_mid_gain = gain;
    }
    void setHighMidGain(float gain)
    {
        high_mid_gain = gain;
    }
    void setTrebleGain(float gain)
    {
        treble_gain = gain;
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

    juce::dsp::IIR::Filter<float> treble_peak;
    float treble_peak_frequency = 5000.0f;
    float treble_peak_q = 0.707f;

    // GUI Parameters
    bool bypass = false;

    float bass_gain = 1.0f;
    float low_mid_gain = 1.0f;
    float high_mid_gain = 1.0f;
    float treble_gain = 1.0f;

    // Smoothed internal parameters
    float smoothed_bass_gain = 1.0f;
    float smoothed_low_mid_gain = 1.0f;
    float smoothed_high_mid_gain = 1.0f;
    float smoothed_treble_gain = 1.0f;

    // Interal parameters
    float smoothing_factor = 0.1f;
};

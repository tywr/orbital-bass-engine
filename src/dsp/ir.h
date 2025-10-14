#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class IRConvolver
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);

    void loadIR();
    void applyGain(juce::AudioBuffer<float>& buffer);
    void setMix(float newMix)
    {
        mix = newMix;
    }
    void setGain(float newGain)
    {
        gain = newGain;
    }
    void setBypass(bool newBypass)
    {
        bypass = newBypass;
    }
    void setFilepath(juce::String newFilepath)
    {
        filepath = newFilepath;
    }
    juce::String getFilepath()
    {
        return filepath;
    }

  private:
    juce::dsp::ProcessSpec processSpec{-1, 0, 0};

    // GUI Parameters
    bool is_ir_loaded = false;
    bool bypass = false;
    float mix = 1.0f;
    float gain = 1.0f;
    juce::String filepath;

    // Internal State
    float previousGain = 1.0f;
    juce::dsp::Convolution convolution;
};

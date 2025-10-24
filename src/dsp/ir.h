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
        float v = juce::jlimit(0.0f, 1.0f, newMix);
        mix.setTargetValue(v);
        raw_mix = newMix;
    }
    void setLevel(float lvl)
    {
        float v = juce::jlimit(0.0f, 2.0f, lvl);
        level.setTargetValue(v);
        raw_level = v;
    }
    void setBypass(bool newBypass)
    {
        bypass = newBypass;
    }
    void setFilepath(juce::String newFilepath)
    {
        filepath = newFilepath;
    }
    void setTypeFromIndex(int index)
    {
        type = index;
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
    juce::String filepath;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> level, mix;

    float raw_mix = 1.0f;
    float raw_level = 1.0f;
    int type = 0;

    juce::dsp::Convolution convolution;
};

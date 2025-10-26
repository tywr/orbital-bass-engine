#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class IRConvolver : juce::dsp::ProcessorBase
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
        raw_mix = newMix;
    }
    void setLevel(float lvl)
    {
        float v = juce::jlimit(0.0f, 2.0f, lvl);
        level.setTargetValue(v);
        raw_level = v;
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
    juce::AudioBuffer<float> dry_buffer;

    // GUI Parameters
    bool is_ir_loaded = false;
    juce::String filepath;

    float smoothing_time = 0.05f;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> level, mix;
    float raw_mix = 1.0f;
    float raw_level = 1.0f;
    int type = 0;
    int loaded_type = -1;

    juce::dsp::Convolution convolution;
};

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class PitchDetector : juce::dsp::ProcessorBase
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    float getPitch(const juce::dsp::ProcessContextReplacing<float>& context);
    void process(
        const juce::dsp::ProcessContextReplacing<float>& context
    ) override
    {
        juce::ignoreUnused(context);
    }
    void reset() override;

  private:
    juce::dsp::ProcessSpec processSpec{1.0f, 1, 1};
};

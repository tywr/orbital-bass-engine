#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include "../../modules/rubberband/rubberband/RubberBandLiveShifter.h"

class PitchShifter : juce::dsp::ProcessorBase
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(
        const juce::dsp::ProcessContextReplacing<float>& context
    ) override;
    void reset() override;
    void setSemitones(int s)
    {
        semitones = s;
    }

  private:
    juce::dsp::ProcessSpec processSpec{44100.0f, 1, 0};
    std::unique_ptr<RubberBand::RubberBandLiveShifter> shifter;
    int semitones = 0;
    juce::AudioBuffer<float> cache_buffer;
};

#pragma once
#include <cmath>

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

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
        pitch_ratio = std::pow(2.0f, float(semitones) / 12.0f);
    }

  private:
    float readFromDelay(float pos);
    juce::dsp::ProcessSpec processSpec{44100.0f, 1, 0};
    int semitones = 0;
    float pitch_ratio = std::pow(2.0f, float(semitones) / 12.0f);
    float crossfade_duration = 0.005f;
    juce::AudioBuffer<float> read_buffer;
    int read_size = 0;
    int crossfade_size = 0;
    float read_pos_a = 0;
    float read_pos_b = 0;
    int write_pos = 0;
    float phase_a = 0.0f;
    float phase_b = 0.5f;
};

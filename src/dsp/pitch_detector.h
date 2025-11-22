#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>

class PitchDetector
{
  public:
    // Buffer size of 8192 samples allows detection down to ~5Hz at 44.1kHz
    // Bass guitar low E is ~41Hz, so this gives plenty of headroom
    static constexpr size_t kBufferSize = 8192;
    static constexpr float kYinThreshold = 0.15f;

    void prepare(const juce::dsp::ProcessSpec& spec);
    float getPitch(const juce::dsp::ProcessContextReplacing<float>& context);
    void reset();

  private:
    float detectPitchYIN();

    juce::dsp::ProcessSpec processSpec{44100.0, 1, 1};
    std::vector<float> inputBuffer;
    std::vector<float> yinBuffer;
    size_t writeIndex = 0;
    float detectedPitch = 0.0f;
};

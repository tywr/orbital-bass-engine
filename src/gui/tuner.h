#pragma once

#include "colours.h"
#include <atomic>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_gui_basics/juce_gui_basics.h>

class Tuner : public juce::Component, public juce::Timer
{
  public:
    Tuner();
    ~Tuner() override;

    void resized() override;
    void paint(juce::Graphics&) override;
    void timerCallback() override;
    void visibilityChanged() override;

    void setPitch(float freq);

    std::function<void()> onClose;

  private:
    void updatePitchDisplay();
    juce::String getNoteLabel(int noteIndex) const;

    juce::TextButton closeButton;

    std::atomic<float> targetFreq{0.0f};
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedFreq{
        0.0f
    };
    float currentFreq = 0.0f;
    float centsDeviation = 0.0f;
    juce::String noteLabel = "-";
    static constexpr float inTuneThreshold = 5.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Tuner)
};

#pragma once

#include "colours.h"
#include <juce_gui_basics/juce_gui_basics.h>

class Tuner : public juce::Component
{
  public:
    Tuner();
    ~Tuner() override;

    void resized() override;
    void paint(juce::Graphics&) override;

    std::function<void()> onClose;

  private:
    juce::TextButton closeButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Tuner)
};

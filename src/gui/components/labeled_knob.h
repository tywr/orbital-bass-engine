#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class LabeledKnob : public juce::Component
{
  public:
    LabeledKnob();
    ~LabeledKnob() override;

    void resized() override;

    // Access to underlying components
    juce::Slider& getSlider() { return slider; }
    juce::Label& getLabel() { return label; }

    // Convenience methods
    void setLabelText(const juce::String& text);
    void setKnobSize(int width, int height);
    void setLabelHeight(int height);

  private:
    juce::Slider slider;
    juce::Label label;

    int knob_width = 60;
    int knob_height = 60;
    int label_height = 20;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabeledKnob)
};

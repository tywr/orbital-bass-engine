#pragma once

#include "base_look_and_feel.h"
#include <juce_gui_basics/juce_gui_basics.h>

class CompressorSelectorLookAndFeel : public BaseLookAndFeel
{
  private:
    float stroke_width = 2.0f;
    juce::Font main_font =
        juce::Font(juce::FontOptions("Oxanium", 12.0f, juce::Font::plain));

  public:
    // CompressorSelectorLookAndFeel();
    void drawLabel(juce::Graphics&, juce::Label&) override;
    void drawRotarySlider(
        juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider
    ) override;
};

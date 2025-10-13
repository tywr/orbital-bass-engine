#pragma once

#include "base_look_and_feel.h"
#include <juce_gui_basics/juce_gui_basics.h>

class AmpSmallLookAndFeel : public BaseLookAndFeel
{
  private:
    float stroke_width = 2.5f;
    juce::Font main_font =
        juce::Font(juce::FontOptions("Oxanium", 11.0f, juce::Font::plain));

  public:
    // AmpSmallLookAndFeel();
    void drawLabel(juce::Graphics&, juce::Label&) override;
    void drawRotarySlider(
        juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider
    ) override;
};

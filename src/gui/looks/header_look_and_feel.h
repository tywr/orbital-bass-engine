#pragma once

#include "base_look_and_feel.h"
#include <juce_gui_basics/juce_gui_basics.h>

class HeaderLookAndFeel : public BaseLookAndFeel
{
  private:
    float stroke_width = 4.0f;
    juce::Font main_font =
        juce::Font(juce::FontOptions("Oxanium", 10.0f, juce::Font::plain))
            .withExtraKerningFactor(0.2f);

  public:
    void drawRotarySlider(
        juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider
    ) override;
};

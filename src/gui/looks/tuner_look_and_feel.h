#pragma once

#include "../colours.h"
#include <juce_gui_basics/juce_gui_basics.h>

class TunerLookAndFeel : public juce::LookAndFeel_V4
{
  private:
    float strokeWidth = 3.0f;
    juce::Font mainFont =
        juce::Font(juce::FontOptions("Oxanium", 11.0f, juce::Font::plain));

  public:
    TunerLookAndFeel();

    juce::LookAndFeel_V4::ColourScheme getColourScheme() const
    {
        return juce::LookAndFeel_V4::ColourScheme(
            ColourCodes::bg,     // windowBackground
            ColourCodes::grey3,  // widgetBackground
            ColourCodes::grey2,  // menuBackground
            ColourCodes::bg,     // outline
            ColourCodes::grey3,  // defaultText
            ColourCodes::bg,     // defaultFill
            ColourCodes::white2, // highlightedText
            ColourCodes::blue0,  // highlightedFill
            ColourCodes::white0  // menuText
        );
    }

    void drawToggleButton(
        juce::Graphics& g, juce::ToggleButton& button, bool isMouseOverButton,
        bool isButtonDown
    ) override;
};

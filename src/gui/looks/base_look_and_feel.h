#pragma once

#include "../colours.h"
#include <juce_gui_basics/juce_gui_basics.h>

class BaseLookAndFeel : public juce::LookAndFeel_V4
{
  private:
    float strokeWidth = 2.0f;
    juce::Font mainFont =
        juce::Font(juce::FontOptions("Typestar", 11.0f, juce::Font::plain));

  public:
    BaseLookAndFeel();

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

    void drawButtonBackground(
        juce::Graphics& g, juce::Button& button,
        const juce::Colour& backgroundColour, bool isMouseOverButton,
        bool isButtonDown
    ) override;

    void drawButtonText(
        juce::Graphics&, juce::TextButton&, bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown
    ) override;

    void drawRotarySlider(
        juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider
    ) override;

    void drawLinearSlider(
        juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider
    ) override;

    void drawTabbedButtonBarBackground(
        juce::TabbedButtonBar& buttonBar, juce::Graphics& g
    ) override;

    void drawTabButton(
        juce::TabBarButton&, juce::Graphics&, bool isMouseOver, bool isMouseDown
    ) override;

    void drawLabel(juce::Graphics& g, juce::Label& label) override;

    juce::Font getSliderPopupFont(juce::Slider&) override
    {
        return mainFont;
    }

    juce::Font getLabelFont(juce::Label&) override
    {
        return mainFont;
    }

    juce::Font getTextButtonFont(juce::TextButton& button, int height) override
    {
        juce::ignoreUnused(button, height);
        return mainFont;
    }

    juce::Font getAlertWindowMessageFont() override
    {
        return mainFont;
    }

    juce::Font getPopupMenuFont() override
    {
        return mainFont;
    }

    juce::Font getComboBoxFont(juce::ComboBox&) override
    {
        return mainFont;
    }
};

#include "compressor_meter_look_and_feel.h"

#include "../colours.h"
#include <juce_gui_basics/juce_gui_basics.h>

CompressorMeterLookAndFeel::CompressorMeterLookAndFeel()
{
    setColourScheme(getColourScheme());
}

void CompressorMeterLookAndFeel::drawLabel(
    juce::Graphics& g, juce::Label& label
)
{
    auto bounds = label.getLocalBounds();
    juce::String text = label.getText();
    g.setFont(main_font);
    g.setColour(label.findColour(juce::Label::textColourId));
    g.drawFittedText(text, bounds, juce::Justification::centred, 1);
}

void CompressorMeterLookAndFeel::drawLinearSlider(
    juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider
)
{
    // g.setColour(GuiColours::COMPRESSOR_BG_COLOUR);
    // g.fillRect(x, y, width, height);

    const float intensity = 0.5f;
    const int numberOfDots = 20;
    const float dotRadius = 4.0f; // Adjust for desired dot size
    const float dotDiameter = dotRadius * 2.0f;
    const float dotSpacingX =
        (width - numberOfDots * dotDiameter) / (numberOfDots - 1);
    const float dotSpacingY = height - 2 * dotDiameter;

    const float totalDotDisplayWidth =
        (numberOfDots * dotDiameter) +
        ((numberOfDots - 1) * (dotSpacingX - dotDiameter));
    const float totalDotDisplayHeight =
        (numberOfDots * dotDiameter) +
        ((numberOfDots - 1) * (dotSpacingY - dotDiameter)); // For vertical

    int dotsToLight = static_cast<int>((sliderPos - x) / numberOfDots);

    const juce::Colour highlightColour =
        slider.findColour(juce::Slider::trackColourId);
    const juce::Colour darkColour = ColourCodes::grey0;

    if (style == juce::Slider::LinearHorizontal)
    {
        float startX = x + (width - totalDotDisplayWidth) * 0.5f;
        float dotY = y + (height - dotDiameter) * 0.5f;

        for (int i = 0; i < numberOfDots; ++i)
        {
            float currentDotX = startX + i * dotSpacingX;

            float dotx = currentDotX - dotRadius;
            float doty_up = dotY + dotSpacingY / 3.0f;
            float doty_down = dotY - dotSpacingY / 3.0f;

            if (i < dotsToLight)
            {
                for (int glow_layer = 3; glow_layer > 0; --glow_layer)
                {
                    float glow_size = dotRadius + (glow_layer * 1.0f);
                    float glow_alpha = (0.15f * intensity) / glow_layer;

                    g.setColour(highlightColour.withAlpha(glow_alpha));
                    g.fillEllipse(
                        dotx - glow_size + dotRadius,
                        doty_up - glow_size + dotRadius, glow_size * 2.0f,
                        glow_size * 2.0f
                    );
                    g.fillEllipse(
                        dotx - glow_size + dotRadius,
                        doty_down - glow_size + dotRadius, glow_size * 2.0f,
                        glow_size * 2.0f
                    );
                }
            }

            if (i < dotsToLight)
                g.setColour(highlightColour);
            else
                g.setColour(darkColour);

            g.fillEllipse(dotx, doty_up, dotDiameter, dotDiameter);
            g.fillEllipse(dotx, doty_down, dotDiameter, dotDiameter);
        }
    }
}

#include "compressor_meter_look_and_feel.h"
#include <cmath>

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
    const float intensity = 0.5f;
    const int numberOfDots = 20;
    const float dotRadius = 4.0f; // Adjust for desired dot size
    const float dotDiameter = dotRadius * 2.0f;

    // spacing between dot edges (so total element width = dots + gaps)
    const float dotSpacingX =
        (width - numberOfDots * dotDiameter) / static_cast<float>(numberOfDots - 1);

    // vertical offset used for splitting up/down dots
    const float dotSpacingY = height - 2.0f * dotDiameter;

    // total width used by the dot grid (dots + gaps)
    const float totalDotDisplayWidth =
        (numberOfDots * dotDiameter) + ((numberOfDots - 1) * dotSpacingX);

    // vertical total (if you later want to use it)
    const float totalDotDisplayHeight =
        (numberOfDots * dotDiameter) +
        ((numberOfDots - 1) * (dotSpacingY - dotDiameter)); // keep if needed

    // map sliderPos (pixels) into 0..1 relative to the slider bounds, then into dots
    float fraction = 0.0f;
    if (width > 0)
        fraction = (sliderPos - static_cast<float>(x)) / static_cast<float>(width);
    fraction = juce::jlimit(0.0f, 1.0f, fraction);

    // how many dots should be lit (0..numberOfDots)
    int dotsToLight = static_cast<int>(std::floor(fraction * static_cast<float>(numberOfDots)));
    dotsToLight = juce::jlimit(0, numberOfDots, dotsToLight);

    const juce::Colour highlightColour =
        slider.findColour(juce::Slider::trackColourId);
    const juce::Colour darkColour = ColourCodes::grey0;

    if (style == juce::Slider::LinearHorizontal)
    {
        // startX is the leftmost *edge* of the first dot, centered in the given bounds
        float startX = x + (width - totalDotDisplayWidth) * 0.5f;

        // centre-y for the pair of dots
        float dotY = y + (height - dotDiameter) * 0.5f;

        // step between left edges of consecutive dots = dotDiameter + dotSpacingX
        const float stepX = dotDiameter + dotSpacingX;

        for (int i = 0; i < numberOfDots; ++i)
        {
            float currentDotLeft = startX + i * stepX;   // left edge of this dot
            float dotx = currentDotLeft;                 // same as left
            // split the pair vertically around dotY:
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
                        doty_up - glow_size + dotRadius,
                        glow_size * 2.0f, glow_size * 2.0f
                    );
                    g.fillEllipse(
                        dotx - glow_size + dotRadius,
                        doty_down - glow_size + dotRadius,
                        glow_size * 2.0f, glow_size * 2.0f
                    );
                }
            }

            g.setColour(i < dotsToLight ? highlightColour : darkColour);

            g.fillEllipse(dotx, doty_up, dotDiameter, dotDiameter);
            g.fillEllipse(dotx, doty_down, dotDiameter, dotDiameter);
        }
    }
}


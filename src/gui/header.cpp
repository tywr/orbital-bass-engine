#include "header.h"

#include "colours.h"
#include "looks/header_look_and_feel.h"
#include "looks/tuner_look_and_feel.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

Header::Header(
    juce::AudioProcessorValueTreeState& params, juce::Value& vin,
    juce::Value& vout, SessionManager& sm
)
    : parameters(params), inputMeter(vin), outputMeter(vout), presetBar(sm)
{
    setLookAndFeel(new HeaderLookAndFeel());

    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);
    inputMeter.setSliderColour(headerColour);
    outputMeter.setSliderColour(headerColour);

    addAndMakeVisible(inputGainSlider);
    inputGainSlider.setSkewFactor(3.0);
    inputGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    inputGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 25);
    inputGainSlider.setColour(
        juce::Slider::rotarySliderFillColourId, headerColour
    );
    inputGainAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "input_gain_db", inputGainSlider
        );

    addAndMakeVisible(outputGainSlider);
    outputGainSlider.setSkewFactor(3.0);
    outputGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    outputGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 25);
    outputGainSlider.setColour(
        juce::Slider::rotarySliderFillColourId, headerColour
    );
    outputGainAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "output_gain_db", outputGainSlider
        );

    addAndMakeVisible(tunerButton);
    tunerButton.setLookAndFeel(new TunerLookAndFeel());
    tunerButton.onClick = [this]()
    {
        if (onTunerClicked)
            onTunerClicked();
    };

    addAndMakeVisible(presetBar);
}

Header::~Header()
{
}

void Header::paint(juce::Graphics& g)
{
    ignoreUnused(g);
}

void Header::resized()
{
    int const padding = 10;
    int const knob_padding = 3 * padding;
    int const knob_size = getHeight() - padding * 2;
    int const meter_width = 6;

    auto bounds = getLocalBounds().reduced(padding);

    // Left side: input meter and gain
    inputMeter.setBounds(bounds.removeFromLeft(meter_width));
    bounds.removeFromLeft(padding);
    inputGainSlider.setBounds(bounds.removeFromLeft(knob_size + knob_padding));
    bounds.removeFromLeft(padding);

    // Right side: output gain and meter
    outputMeter.setBounds(bounds.removeFromRight(meter_width));
    bounds.removeFromRight(padding);
    outputGainSlider.setBounds(bounds.removeFromRight(knob_size + knob_padding));
    bounds.removeFromRight(padding);

    // Center area: tuner button, load/save icons, and preset slots
    auto centerBounds = bounds;

    int const iconButtonSize = centerBounds.getHeight() - 4;
    int const spacing = 6;

    // Tuner button - same size as preset icon buttons
    tunerButton.setBounds(centerBounds.removeFromLeft(iconButtonSize));
    centerBounds.removeFromLeft(spacing);

    // Preset bar takes the remaining space
    presetBar.setBounds(centerBounds);
}

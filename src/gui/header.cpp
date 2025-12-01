#include "header.h"

#include "colours.h"
#include "looks/tuner_look_and_feel.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

Header::Header(
    juce::AudioProcessorValueTreeState& params, juce::Value& vin,
    juce::Value& vout, SessionManager& sm
)
    : parameters(params), inputMeter(vin), outputMeter(vout),
      sessionNameDisplay(sm), presetBar(sm)
{
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

    addAndMakeVisible(presetIconButtons);
    addAndMakeVisible(sessionNameDisplay);
    addAndMakeVisible(presetBar);
}

Header::~Header()
{
}

void Header::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Calculate section bounds
    int const padding = 10;
    int const knob_padding = 3 * padding;
    int const knob_size = getHeight() - padding * 2;
    int const meter_width = 6;

    float leftSeparatorX = padding + meter_width + padding + knob_size + knob_padding + padding;
    float rightSeparatorX = getWidth() - padding - meter_width - padding - knob_size - knob_padding - padding;

    // Draw black background behind input slider section
    juce::Rectangle<float> inputSection(0.0f, 0.0f, leftSeparatorX, (float)getHeight());
    g.setColour(juce::Colours::black);
    g.fillRect(inputSection);

    // Draw grey background behind center section (tuner)
    juce::Rectangle<float> centerSection(leftSeparatorX, 0.0f, rightSeparatorX - leftSeparatorX, (float)getHeight());
    g.setColour(ColourCodes::bg2);
    g.fillRect(centerSection);

    // Draw black background behind output slider section
    juce::Rectangle<float> outputSection(rightSeparatorX, 0.0f, getWidth() - rightSeparatorX, (float)getHeight());
    g.setColour(juce::Colours::black);
    g.fillRect(outputSection);

    // Draw solid grey frame around entire header
    g.setColour(ColourCodes::grey3);
    g.drawRect(bounds, 2.0f);

    // Draw separators between sections
    float separatorY1 = 0.0f;
    float separatorY2 = (float)getHeight();

    g.setColour(ColourCodes::grey3);
    g.drawLine(leftSeparatorX, separatorY1, leftSeparatorX, separatorY2, 2.0f);
    g.drawLine(rightSeparatorX, separatorY1, rightSeparatorX, separatorY2, 2.0f);

    // Draw separator between preset icon buttons and session name
    if (presetIconButtons.isVisible() && sessionNameDisplay.isVisible())
    {
        auto iconButtonsBounds = presetIconButtons.getBounds();
        auto sessionNameBounds = sessionNameDisplay.getBounds();
        float separator1X = iconButtonsBounds.getRight() + (sessionNameBounds.getX() - iconButtonsBounds.getRight()) / 2.0f;
        g.drawLine(separator1X, separatorY1, separator1X, separatorY2, 2.0f);
    }

    // Draw separator between session name and preset bar
    if (sessionNameDisplay.isVisible() && presetBar.isVisible())
    {
        auto sessionNameBounds = sessionNameDisplay.getBounds();
        auto presetBarBounds = presetBar.getBounds();
        float separator2X = sessionNameBounds.getRight() + (presetBarBounds.getX() - sessionNameBounds.getRight()) / 2.0f;
        g.drawLine(separator2X, separatorY1, separator2X, separatorY2, 2.0f);
    }
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

    // Center area: tuner button and preset controls
    int const iconButtonSize = bounds.getHeight() - 4;
    int const spacing = 10;

    // Tuner button on the left
    tunerButton.setBounds(bounds.removeFromLeft(iconButtonSize));
    bounds.removeFromLeft(spacing);

    // Preset icon buttons
    int const iconButtonsWidth = (iconButtonSize + spacing) * 2;
    presetIconButtons.setBounds(bounds.removeFromLeft(iconButtonsWidth));
    bounds.removeFromLeft(spacing);

    // Session name display
    int const sessionNameWidth = 120;
    sessionNameDisplay.setBounds(bounds.removeFromLeft(sessionNameWidth));
    bounds.removeFromLeft(spacing);

    // Preset bar takes remaining space
    presetBar.setBounds(bounds);
}

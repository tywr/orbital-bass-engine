#include "labeled_knob.h"

LabeledKnob::LabeledKnob()
{
    addAndMakeVisible(slider);
    addAndMakeVisible(label);

    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 1, 1);

    label.setJustificationType(juce::Justification::centred);
}

LabeledKnob::~LabeledKnob()
{
}

void LabeledKnob::resized()
{
    auto bounds = getLocalBounds().withSizeKeepingCentre(
        getWidth(), label_height + 1.5f * knob_height
    );

    // Label on top
    label.setBounds(bounds.removeFromTop(label_height));

    // Slider below, centered
    slider.setBounds(bounds.withSizeKeepingCentre(knob_width, knob_height));
}

void LabeledKnob::setLabelText(const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
}

void LabeledKnob::setKnobSize(int width, int height)
{
    knob_width = width;
    knob_height = height;
    resized();
}

void LabeledKnob::setLabelHeight(int height)
{
    label_height = height;
    resized();
}

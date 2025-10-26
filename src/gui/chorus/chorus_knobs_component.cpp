#include "chorus_knobs_component.h"
#include "../colours.h"
#include "../looks/compressor_look_and_feel.h"
#include "../looks/compressor_selector_look_and_feel.h"
#include "chorus_dimensions.h"

ChorusKnobsComponent::ChorusKnobsComponent(
    juce::AudioProcessorValueTreeState& params
)
    : parameters(params)
{
    for (auto knob : knobs)
    {
        addAndMakeVisible(knob.slider);
        addAndMakeVisible(knob.label);
        knob.label->setText(knob.label_text, juce::dontSendNotification);
        knob.label->setJustificationType(juce::Justification::centred);
        knob.slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knob.slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 1, 1);
        knob.label->setColour(
            juce::Slider::textBoxOutlineColourId,
            juce::Colours::transparentBlack
        );
        knob.label->setColour(
            juce::Slider::textBoxTextColourId, ColourCodes::grey3
        );
        slider_attachments.push_back(
            std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                parameters, knob.parameter_id, *knob.slider
            )
        );
    }
}

ChorusKnobsComponent::~ChorusKnobsComponent()
{
}

void ChorusKnobsComponent::paint(juce::Graphics& g)
{
    juce::ignoreUnused(g);
}

void ChorusKnobsComponent::resized()
{

    auto bounds = getLocalBounds();
    auto label_bounds = bounds.removeFromTop(ChorusDimensions::LABEL_HEIGHT);
    const int knob_box_size = bounds.getWidth() / (int)knobs.size();
    for (auto knob : knobs)
    {
        knob.label->setBounds(label_bounds.removeFromLeft(knob_box_size)
                                  .withSizeKeepingCentre(
                                      ChorusDimensions::KNOB_SIZE,
                                      ChorusDimensions::LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(bounds.removeFromLeft(knob_box_size)
                                   .withSizeKeepingCentre(
                                       ChorusDimensions::KNOB_SIZE,
                                       ChorusDimensions::KNOB_SIZE
                                   ));
    }
}

void ChorusKnobsComponent::switchColour(
    juce::Colour colour1, juce::Colour colour2
)
{
    juce::ignoreUnused(colour2);
    for (ChorusKnob knob : knobs)
    {
        knob.slider->setColour(juce::Slider::rotarySliderFillColourId, colour1);
    }
    repaint();
}

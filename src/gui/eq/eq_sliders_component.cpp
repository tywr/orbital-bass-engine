#include "eq_sliders_component.h"
#include "../colours.h"
#include "../components/solid_tooltip.h"
#include "eq_dimensions.h"

EqSlidersComponent::EqSlidersComponent(
    juce::AudioProcessorValueTreeState& params
)
    : parameters(params)
{
    for (auto slider : sliders)
    {
        addAndMakeVisible(slider.knob);
        slider.knob->setLabelText(slider.label_text);
        slider.knob->setKnobSize(EqDimensions::KNOB_SIZE, EqDimensions::KNOB_SIZE);
        slider.knob->setLabelHeight(EqDimensions::LABEL_HEIGHT);
        slider.knob->getLabel().setColour(
            juce::Slider::textBoxOutlineColourId,
            juce::Colours::transparentBlack
        );
        slider.knob->getLabel().setColour(
            juce::Slider::textBoxTextColourId, ColourCodes::grey3
        );
        slider_attachments.push_back(
            std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                parameters, slider.parameter_id, slider.knob->getSlider()
            )
        );
    }
}

EqSlidersComponent::~EqSlidersComponent()
{
}

void EqSlidersComponent::paint(juce::Graphics& g)
{
    juce::ignoreUnused(g);
}

void EqSlidersComponent::resized()
{
    auto bounds = getLocalBounds();
    const int slider_box_size = bounds.getWidth() / (int)sliders.size();
    for (auto slider : sliders)
    {
        slider.knob->setBounds(bounds.removeFromLeft(slider_box_size));
    }
}

void EqSlidersComponent::switchColour(
    juce::Colour colour1, juce::Colour colour2
)
{
    juce::ignoreUnused(colour2);
    for (EqSlider slider : sliders)
    {
        slider.knob->getSlider().setColour(
            juce::Slider::rotarySliderFillColourId, colour1
        );
    }
    repaint();
}

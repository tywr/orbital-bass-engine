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
        addAndMakeVisible(slider.slider);
        addAndMakeVisible(slider.label);
        slider.label->setText(slider.label_text, juce::dontSendNotification);
        slider.label->setJustificationType(juce::Justification::centred);
        slider.slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, EqDimensions::LABEL_HEIGHT);
        slider.label->setColour(
            juce::Slider::textBoxOutlineColourId,
            juce::Colours::transparentBlack
        );
        slider.label->setColour(
            juce::Slider::textBoxTextColourId, ColourCodes::grey3
        );
        slider_attachments.push_back(
            std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                parameters, slider.parameter_id, *slider.slider
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
    auto label_bounds = bounds.removeFromTop(EqDimensions::LABEL_HEIGHT);
    const int slider_box_size = bounds.getWidth() / (int)sliders.size();
    for (auto slider : sliders)
    {
        slider.label->setBounds(label_bounds.removeFromLeft(slider_box_size)
                                    .withSizeKeepingCentre(
                                        EqDimensions::LABEL_WIDTH,
                                        EqDimensions::LABEL_HEIGHT
                                    ));
        slider.slider->setBounds(bounds.removeFromLeft(slider_box_size)
                                     .withSizeKeepingCentre(
                                         EqDimensions::KNOB_SIZE,
                                         EqDimensions::KNOB_SIZE + EqDimensions::LABEL_HEIGHT
                                     ));
    }
}

void EqSlidersComponent::switchColour(
    juce::Colour colour1, juce::Colour colour2
)
{
    juce::ignoreUnused(colour2);
    for (EqSlider slider : sliders)
    {
        slider.slider->setColour(
            juce::Slider::rotarySliderFillColourId, colour1
        );
    }
    repaint();
}

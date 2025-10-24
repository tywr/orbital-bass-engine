#include "ir_component.h"
#include "../colours.h"
#include "ir_dimensions.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

IRComponent::IRComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params)
{

    addAndMakeVisible(bypassButton);
    bypassButton.setButtonText("bypass");
    bypassButton.onClick = [this]() { switchColour(); };
    bypassButtonAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "ir_bypass", bypassButton
        );

    type_slider_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "ir_type", type_slider
        );

    for (auto type : types)
    {
        addAndMakeVisible(type.button);
        type.button->onClick = [this, type]() { switchIR(type); };
    }
    initType();

    addAndMakeVisible(ir_mix_slider);
    addAndMakeVisible(ir_mix_label);

    ir_mix_label.setText("mix", juce::dontSendNotification);
    ir_mix_label.setJustificationType(juce::Justification::centred);
    ir_mix_label.attachToComponent(&ir_mix_slider, false);
    ir_mix_slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    ir_mix_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    ir_mix_slider.setColour(
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    ir_mix_slider.setColour(
        juce::Slider::textBoxTextColourId, ColourCodes::grey3
    );
    ir_mix_slider.setColour(
        juce::Slider::rotarySliderFillColourId, ColourCodes::white0
    );
    ir_mix_sliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "ir_mix", ir_mix_slider
        );

    addAndMakeVisible(gain_slider);
    addAndMakeVisible(gain_label);
    gain_label.setText("gain", juce::dontSendNotification);
    gain_label.setJustificationType(juce::Justification::centred);
    gain_label.attachToComponent(&gain_slider, false);
    gain_slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gain_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    gain_slider.setColour(
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    gain_slider.setColour(
        juce::Slider::textBoxTextColourId, ColourCodes::grey3
    );
    gain_slider.setColour(
        juce::Slider::rotarySliderFillColourId, ColourCodes::white0
    );
    gain_sliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "ir_level", gain_slider
        );

    switchColour();
}

void IRComponent::initType()
{
    size_t current_index = static_cast<size_t>(type_slider.getValue());
    auto current_type = types[current_index];
    for (auto type : types)
    {
        if (type.id == current_type.id)
        {
            selected_type = current_type;
            type.button->setToggleState(true, juce::dontSendNotification);
        }
        else
        {
            type.button->setToggleState(false, juce::dontSendNotification);
        }
    }
}

void IRComponent::switchIR(IRType new_type)
{
    selected_type = new_type;
    double index = 0;
    for (auto type : types)
    {
        if (type.id == new_type.id)
        {
            type_slider.setValue(index);
            type.button->setToggleState(true, juce::dontSendNotification);
            type.button->setEnabled(false);
        }
        else
        {
            type.button->setToggleState(false, juce::dontSendNotification);
            type.button->setEnabled(true);
        }
        index += 1;
    }
    repaint();
}

IRComponent::~IRComponent()
{
}

void IRComponent::paint(juce::Graphics& g)
{
    ignoreUnused(g);
}

void IRComponent::resized()
{
    auto bounds = getLocalBounds();

    auto type_bounds =
        bounds.removeFromTop(IRDimensions::IR_TYPE_BUTTONS_HEIGHT);
    int const type_size =
        type_bounds.getWidth() / static_cast<int>(types.size());
    for (auto type : types)
    {
        type.button->setBounds(
            type_bounds.removeFromLeft(type_size).withSizeKeepingCentre(
                IRDimensions::IR_TYPE_BUTTON_SIZE,
                IRDimensions::IR_TYPE_BUTTON_SIZE
            )
        );
    }

    auto ir_bounds =
        bounds.withSizeKeepingCentre(IRDimensions::WIDTH, IRDimensions::HEIGHT);

    auto middle_bounds = ir_bounds.withSizeKeepingCentre(
        IRDimensions::WIDTH - 2 * IRDimensions::SIDE_PADDING,
        IRDimensions::HEIGHT
    );

    const int knob_size = middle_bounds.getWidth() / 3;

    gain_slider.setBounds(middle_bounds.removeFromLeft(knob_size));
    ir_mix_slider.setBounds(middle_bounds.removeFromRight(knob_size));

    auto left_bounds = ir_bounds.removeFromLeft(IRDimensions::SIDE_PADDING);
    bypassButton.setBounds(left_bounds.withSizeKeepingCentre(
        IRDimensions::BYPASS_SIZE, IRDimensions::BYPASS_SIZE
    ));
}

void IRComponent::switchColour()
{
    if (bypassButton.getToggleState())
    {
        iRColour = ColourCodes::grey3;
    }
    else
    {
        iRColour = ColourCodes::white0;
    }
    ir_mix_slider.setColour(juce::Slider::rotarySliderFillColourId, iRColour);
    gain_slider.setColour(juce::Slider::rotarySliderFillColourId, iRColour);
    repaint();
}

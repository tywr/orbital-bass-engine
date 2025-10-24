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

    addAndMakeVisible(statusLabel);
    statusLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(irMixSlider);
    addAndMakeVisible(irMixLabel);
    irMixLabel.setText("mix", juce::dontSendNotification);
    irMixLabel.setJustificationType(juce::Justification::centred);
    irMixLabel.attachToComponent(&irMixSlider, false);
    irMixSlider.setRange(0.0, 1.0, 0.01);
    irMixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    irMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    irMixSlider.setColour(
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    irMixSlider.setColour(
        juce::Slider::textBoxTextColourId, ColourCodes::grey3
    );
    irMixSlider.setColour(
        juce::Slider::rotarySliderFillColourId, ColourCodes::white0
    );
    irMixSliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "ir_mix", irMixSlider
        );

    addAndMakeVisible(gainSlider);
    addAndMakeVisible(gainLabel);
    gainLabel.setText("gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.attachToComponent(&gainSlider, false);
    gainSlider.setRange(0.0, 1.0, 0.01);
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    gainSlider.setColour(
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    gainSlider.setColour(juce::Slider::textBoxTextColourId, ColourCodes::grey3);
    gainSlider.setColour(
        juce::Slider::rotarySliderFillColourId, ColourCodes::white0
    );
    gainSliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "ir_level", gainSlider
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
    const int xpadding = 50;
    const int ypadding = 50;
    const int load_button_height = 30;
    const int button_size = 50;
    const int label_padding = 20;
    const int inner_knob_padding = 60;

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

    gainSlider.setBounds(middle_bounds.removeFromLeft(knob_size));
    irMixSlider.setBounds(middle_bounds.removeFromRight(knob_size));

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
    irMixSlider.setColour(juce::Slider::rotarySliderFillColourId, iRColour);
    gainSlider.setColour(juce::Slider::rotarySliderFillColourId, iRColour);
    repaint();
}

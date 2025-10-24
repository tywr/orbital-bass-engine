#include "ir_component.h"
#include "../colours.h"
#include "ir_dimensions.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

IRComponent::IRComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params)
{
    addAndMakeVisible(drag_tooltip);
    drag_tooltip.setJustificationType(juce::Justification::centred);
    drag_tooltip.setAlwaysOnTop(true);
    drag_tooltip.setColour(juce::Label::backgroundColourId, ColourCodes::bg0);

    addAndMakeVisible(bypassButton);
    bypassButton.setButtonText("bypass");
    bypassButton.setColour(
        juce::ToggleButton::tickColourId, ColourCodes::grey3
    );
    bypassButton.setColour(
        juce::ToggleButton::tickDisabledColourId, ColourCodes::white0
    );
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
        type.button->setColour(
            juce::ToggleButton::textColourId, ColourCodes::white0
        );
    }
    initType();

    addAndMakeVisible(ir_mix_slider);
    addAndMakeVisible(ir_mix_label);

    ir_mix_label.setText("mix", juce::dontSendNotification);
    ir_mix_label.setJustificationType(juce::Justification::centred);
    ir_mix_slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    ir_mix_slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    ir_mix_slider.setColour(
        juce::Slider::rotarySliderFillColourId, ColourCodes::white0
    );
    ir_mix_sliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "ir_mix", ir_mix_slider
        );

    addAndMakeVisible(gain_slider);
    addAndMakeVisible(gain_label);
    gain_label.setText("level", juce::dontSendNotification);
    gain_label.setJustificationType(juce::Justification::centred);
    gain_slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gain_slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    gain_slider.setColour(
        juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack
    );
    gain_sliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "ir_level", gain_slider
        );

    setupSliderTooltipHandling(&ir_mix_slider, &ir_mix_label);
    setupSliderTooltipHandling(&gain_slider, &gain_label);
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
    auto bounds = getLocalBounds().toFloat();
    bounds.removeFromTop(IRDimensions::IR_TYPE_BUTTONS_HEIGHT);
    auto ir_bounds =
        bounds.withSizeKeepingCentre(IRDimensions::WIDTH, IRDimensions::HEIGHT);
    g.setColour(ColourCodes::bg0);
    g.fillRoundedRectangle(ir_bounds, IRDimensions::CORNER_RADIUS);
    g.setColour(current_colour);
    g.drawRoundedRectangle(
        ir_bounds, IRDimensions::CORNER_RADIUS, IRDimensions::BORDER_THICKNESS
    );
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
    auto knob_bounds = middle_bounds.withSizeKeepingCentre(
        middle_bounds.getWidth(), IRDimensions::BOX_HEIGHT
    );
    auto label_bounds = knob_bounds.removeFromTop(IRDimensions::LABEL_HEIGHT);
    ir_mix_label.setBounds(
        label_bounds.removeFromRight(IRDimensions::KNOB_SIZE)
    );
    gain_label.setBounds(label_bounds.removeFromLeft(IRDimensions::KNOB_SIZE));

    gain_slider.setBounds(knob_bounds.removeFromLeft(IRDimensions::KNOB_SIZE));
    ir_mix_slider.setBounds(
        knob_bounds.removeFromRight(IRDimensions::KNOB_SIZE)
    );

    auto left_bounds = ir_bounds.removeFromLeft(IRDimensions::SIDE_PADDING);
    bypassButton.setBounds(left_bounds.withSizeKeepingCentre(
        IRDimensions::BYPASS_SIZE, IRDimensions::BYPASS_SIZE
    ));
}

void IRComponent::switchColour()
{
    if (bypassButton.getToggleState())
    {
        current_colour = ColourCodes::grey3;
    }
    else
    {
        current_colour = ColourCodes::white0;
    }
    ir_mix_slider.setColour(
        juce::Slider::rotarySliderFillColourId, current_colour
    );
    gain_slider.setColour(
        juce::Slider::rotarySliderFillColourId, current_colour
    );
    repaint();
}

void IRComponent::setupSliderTooltipHandling(
    juce::Slider* slider, juce::Label* label
)
{

    slider->onDragStart = [this, slider = slider, label = label]()
    {
        slider_being_dragged = true;
        drag_tooltip.setVisible(false);
        // delay using a Timer
        juce::Timer::callAfterDelay(
            300,
            [this, slider, label]()
            {
                if (slider->isMouseButtonDown())
                {
                    drag_tooltip.setText(
                        juce::String(slider->getValue(), 2),
                        juce::dontSendNotification
                    );
                    drag_tooltip.setBounds(
                        label->getX(), label->getY(), label->getWidth(),
                        label->getHeight()
                    );
                    drag_tooltip.toFront(true);
                    drag_tooltip.setVisible(true);
                    drag_tooltip.repaint();
                }
            }
        );
    };
    slider->onDragEnd = [this]()
    {
        slider_being_dragged = false;
        drag_tooltip.setVisible(false);
    };
    slider->onValueChange = [this, slider = slider]()
    {
        if (slider_being_dragged && drag_tooltip.isVisible())
            drag_tooltip.setText(
                juce::String(slider->getValue(), 2), juce::dontSendNotification
            );
    };
}

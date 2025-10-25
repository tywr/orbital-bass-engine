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

    addAndMakeVisible(type_display_label);
    auto* parameter = parameters.getParameter("ir_type");
    type_display_label_attachment = std::make_unique<juce::ParameterAttachment>(
        *parameter,
        [this, parameter](float new_value)
        {
            juce::StringArray values = parameter->getAllValueStrings();
            juce::String text = values[(int)new_value];

            type_display_label.setText(text, juce::dontSendNotification);
        }
    );
    type_display_label_attachment->sendInitialUpdate();
    // juce::Value ir_type_value = parameters.getParameterAsValue("ir_type");
    // type_display_label.getTextValue().referTo(ir_type_value);

    for (auto knob : knobs)
    {
        addAndMakeVisible(knob.slider);
        addAndMakeVisible(knob.label);
        knob.slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knob.label->setText(knob.label_text, juce::dontSendNotification);
        knob.slider->setTextBoxStyle(
            juce::Slider::NoTextBox, false, IRDimensions::KNOB_SIZE,
            IRDimensions::KNOB_SIZE
        );
        knob.slider->setColour(
            juce::Slider::rotarySliderFillColourId, ColourCodes::white0
        );
        slider_attachments.push_back(
            std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                parameters, knob.parameter_id, *knob.slider
            )
        );
        setupSliderTooltipHandling(knob.slider, knob.label);
    }
    switchColour();
}

IRComponent::~IRComponent()
{
}

void IRComponent::paint(juce::Graphics& g)
{
    auto bounds =
        getLocalBounds()
            .withSizeKeepingCentre(IRDimensions::WIDTH, IRDimensions::HEIGHT)
            .toFloat();
    g.setColour(ColourCodes::bg0);
    g.fillRoundedRectangle(bounds, IRDimensions::CORNER_RADIUS);
    g.setColour(current_colour);
    g.drawRoundedRectangle(
        bounds, IRDimensions::CORNER_RADIUS, IRDimensions::BORDER_THICKNESS
    );
}

void IRComponent::resized()
{
    auto bounds = getLocalBounds().withSizeKeepingCentre(
        IRDimensions::WIDTH, IRDimensions::HEIGHT
    );
    auto middle_bounds = bounds.withSizeKeepingCentre(
        IRDimensions::WIDTH - 2 * IRDimensions::SIDE_WIDTH, IRDimensions::HEIGHT
    );
    auto knob_bounds = middle_bounds.withSizeKeepingCentre(
        middle_bounds.getWidth(), IRDimensions::BOX_HEIGHT
    );
    auto label_bounds = knob_bounds.removeFromTop(IRDimensions::LABEL_HEIGHT);

    const int knob_box_size = knob_bounds.getWidth() / (knobs.size());
    for (size_t i = 0; i < 3; ++i)
    {
        IRKnob knob = knobs[i];
        knob.label->setBounds(label_bounds.removeFromLeft(knob_box_size)
                                  .withSizeKeepingCentre(
                                      knob_box_size, IRDimensions::LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(knob_bounds.removeFromLeft(knob_box_size)
                                   .withSizeKeepingCentre(
                                       IRDimensions::KNOB_SIZE,
                                       IRDimensions::KNOB_SIZE
                                   ));
    }

    auto left_bounds = bounds.removeFromLeft(IRDimensions::SIDE_WIDTH);
    bypassButton.setBounds(left_bounds.withSizeKeepingCentre(
        IRDimensions::BYPASS_SIZE, IRDimensions::BYPASS_SIZE
    ));

    auto right_bounds = bounds.removeFromRight(IRDimensions::SIDE_WIDTH);
    type_display_label.setBounds(right_bounds.withSizeKeepingCentre(
        IRDimensions::IR_TYPE_BUTTONS_HEIGHT,
        IRDimensions::IR_TYPE_BUTTONS_HEIGHT
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

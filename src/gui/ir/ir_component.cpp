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
        juce::ToggleButton::tickColourId, GuiColours::DEFAULT_INACTIVE_COLOUR
    );
    bypassButton.setColour(
        juce::ToggleButton::tickDisabledColourId, ColourCodes::white0
    );
    bypassButton.onClick = [this]() { switchColour(); };
    bypassButtonAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "ir_bypass", bypassButton
        );

    addAndMakeVisible(type_display);
    type_display.setFont(
        juce::Font(juce::FontOptions("Oxanium", 24.0f, juce::Font::plain)), true
    );
    type_display.setJustification(juce::Justification::centred);
    type_display.setColour(ColourCodes::grey3);
    auto* parameter = parameters.getParameter("ir_type");
    type_display_attachment = std::make_unique<juce::ParameterAttachment>(
        *parameter,
        [this, parameter](float new_value)
        {
            juce::StringArray values = parameter->getAllValueStrings();
            juce::String text = values[(int)new_value];
            type_display.setText(text);
            repaint();
        }
    );
    type_display_attachment->sendInitialUpdate();

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
            juce::Slider::rotarySliderFillColourId, ColourCodes::grey3
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
    bool bypass = bypassButton.getToggleState();
    juce::Colour colour;
    if (!bypass)
    {
        colour = ColourCodes::white0;
    }
    else
    {
        colour = GuiColours::DEFAULT_INACTIVE_COLOUR;
    }
    auto outer_bounds =
        getLocalBounds()
            .withSizeKeepingCentre(IRDimensions::WIDTH, IRDimensions::HEIGHT)
            .toFloat();
    auto inner_bounds =
        outer_bounds.reduced(IRDimensions::BORDER_THICKNESS).toFloat();

    g.setColour(ColourCodes::bg0);
    g.fillRoundedRectangle(inner_bounds, IRDimensions::CORNER_RADIUS);

    juce::Path border_path;
    border_path.addRoundedRectangle(
        outer_bounds,
        IRDimensions::CORNER_RADIUS + IRDimensions::BORDER_THICKNESS
    );
    border_path.addRoundedRectangle(inner_bounds, IRDimensions::CORNER_RADIUS);
    border_path.setUsingNonZeroWinding(false);
    g.setColour(colour);
    g.fillPath(border_path);

    auto right_bounds =
        inner_bounds.removeFromRight(IRDimensions::SIDE_WIDTH)
            .withSizeKeepingCentre(
                IRDimensions::IR_LABEL_WIDTH, IRDimensions::IR_LABEL_HEIGHT
            );
    type_display.setBoundingBox(right_bounds);
    type_display.draw(g, 1.0f);
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

    const int knob_box_size = knob_bounds.getWidth() / (int)knobs.size();
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
    type_display.setBounds(right_bounds.withSizeKeepingCentre(
        IRDimensions::IR_LABEL_WIDTH, IRDimensions::IR_LABEL_HEIGHT
    ));
}

void IRComponent::switchColour()
{
    if (bypassButton.getToggleState())
    {
        current_colour = GuiColours::DEFAULT_INACTIVE_COLOUR;
    }
    else
    {
        current_colour = ColourCodes::white0;
    }
    for (auto knob : knobs)
    {
        knob.slider->setColour(
            juce::Slider::rotarySliderFillColourId, current_colour
        );
    }
    type_display.setColour(current_colour);
    repaint();
}

void IRComponent::setupSliderTooltipHandling(
    juce::Slider* slider, juce::Label* label
)
{

    slider->onDragStart = [this, sl = slider, lab = label]()
    {
        slider_being_dragged = true;
        drag_tooltip.setVisible(false);
        // delay using a Timer
        juce::Timer::callAfterDelay(
            300,
            [this, sl, lab]()
            {
                if (sl->isMouseButtonDown())
                {
                    drag_tooltip.setText(
                        juce::String(sl->getValue(), 2),
                        juce::dontSendNotification
                    );
                    drag_tooltip.setBounds(
                        lab->getX(), lab->getY(), lab->getWidth(),
                        lab->getHeight()
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
    slider->onValueChange = [this, sl = slider]()
    {
        if (slider_being_dragged && drag_tooltip.isVisible())
            drag_tooltip.setText(
                juce::String(sl->getValue(), 2), juce::dontSendNotification
            );
    };
}

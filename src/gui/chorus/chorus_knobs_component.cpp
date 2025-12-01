#include "chorus_knobs_component.h"
#include "../colours.h"
#include "../components/solid_tooltip.h"
#include "chorus_dimensions.h"

ChorusKnobsComponent::ChorusKnobsComponent(
    juce::AudioProcessorValueTreeState& params
)
    : parameters(params)
{
    drag_tooltip.setJustificationType(juce::Justification::centred);
    drag_tooltip.setAlwaysOnTop(true);
    drag_tooltip.setColour(
        juce::Label::backgroundColourId, GuiColours::AMP_BG_COLOUR
    );
    addAndMakeVisible(drag_tooltip);
    slider_being_dragged = false;
    drag_tooltip.setVisible(false);

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
        knob.slider->onDragStart =
            [this, slider = knob.slider, label = knob.label]()
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
        knob.slider->onDragEnd = [this]()
        {
            slider_being_dragged = false;
            drag_tooltip.setVisible(false);
        };
        knob.slider->onValueChange = [this, slider = knob.slider]()
        {
            if (slider_being_dragged && drag_tooltip.isVisible())
                drag_tooltip.setText(
                    juce::String(slider->getValue(), 2),
                    juce::dontSendNotification
                );
        };
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

    // Top row: rate, depth
    auto top_row_bounds = bounds.removeFromTop(bounds.getHeight() / 2);
    auto top_label_bounds = top_row_bounds.removeFromTop(ChorusDimensions::LABEL_HEIGHT);
    const int top_knob_box_size = top_row_bounds.getWidth() / 2;

    for (size_t i = 0; i < 2; ++i) // rate, depth
    {
        ChorusKnob knob = knobs[i];
        knob.label->setBounds(top_label_bounds.removeFromLeft(top_knob_box_size)
                                  .withSizeKeepingCentre(
                                      ChorusDimensions::KNOB_SIZE,
                                      ChorusDimensions::LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(top_row_bounds.removeFromLeft(top_knob_box_size)
                                   .withSizeKeepingCentre(
                                       ChorusDimensions::KNOB_SIZE,
                                       ChorusDimensions::KNOB_SIZE
                                   ));
    }

    // Bottom row: crossover, mix
    auto bottom_label_bounds = bounds.removeFromTop(ChorusDimensions::LABEL_HEIGHT);
    const int bottom_knob_box_size = bounds.getWidth() / 2;

    for (size_t i = 2; i < 4; ++i) // crossover, mix
    {
        ChorusKnob knob = knobs[i];
        knob.label->setBounds(bottom_label_bounds.removeFromLeft(bottom_knob_box_size)
                                  .withSizeKeepingCentre(
                                      ChorusDimensions::KNOB_SIZE,
                                      ChorusDimensions::LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(bounds.removeFromLeft(bottom_knob_box_size)
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

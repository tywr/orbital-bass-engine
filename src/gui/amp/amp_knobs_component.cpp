#include "amp_knobs_component.h"
#include "../colours.h"
#include "amp_dimensions.h"

AmpKnobsComponent::AmpKnobsComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params)
{
    drag_tooltip.setJustificationType(juce::Justification::centred);
    drag_tooltip.setAlwaysOnTop(true);
    drag_tooltip.setColour(
        juce::Label::backgroundColourId, GuiColours::AMP_BG_COLOUR
    );
}

AmpKnobsComponent::~AmpKnobsComponent()
{
}

void AmpKnobsComponent::paint(juce::Graphics& g)
{
    juce::ignoreUnused(g);
}

void AmpKnobsComponent::resized()
{
    auto bounds = getLocalBounds();

    // Left side: drive knob alone
    auto left_bounds = bounds.removeFromLeft(bounds.getWidth() / 4);
    auto drive_label_bounds = left_bounds.removeFromTop(AmpDimensions::AMP_LABEL_HEIGHT);
    AmpKnob drive_knob = current_knobs[0]; // drive
    drive_knob.label->setBounds(drive_label_bounds.withSizeKeepingCentre(
        AmpDimensions::AMP_SMALL_KNOB_WIDTH,
        AmpDimensions::AMP_LABEL_HEIGHT
    ));
    drive_knob.slider->setBounds(left_bounds.withSizeKeepingCentre(
        AmpDimensions::AMP_SMALL_KNOB_WIDTH,
        AmpDimensions::AMP_SMALL_KNOB_HEIGHT
    ));

    // Right side: two rows of 3 knobs each
    auto right_bounds = bounds;

    // Top row: era, grunt, attack
    auto top_row_bounds = right_bounds.removeFromTop(right_bounds.getHeight() / 2);
    auto top_label_bounds = top_row_bounds.removeFromTop(AmpDimensions::AMP_LABEL_HEIGHT);
    const int top_knob_box_size = top_row_bounds.getWidth() / 3;

    for (size_t i = 1; i <= 3; ++i) // era, grunt, attack
    {
        AmpKnob knob = current_knobs[i];
        knob.label->setBounds(top_label_bounds.removeFromLeft(top_knob_box_size)
                                  .withSizeKeepingCentre(
                                      AmpDimensions::AMP_SMALL_KNOB_WIDTH,
                                      AmpDimensions::AMP_LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(top_row_bounds.removeFromLeft(top_knob_box_size)
                                   .withSizeKeepingCentre(
                                       AmpDimensions::AMP_SMALL_KNOB_WIDTH,
                                       AmpDimensions::AMP_SMALL_KNOB_HEIGHT
                                   ));
    }

    // Bottom row: level, mix, master
    auto bottom_label_bounds = right_bounds.removeFromTop(AmpDimensions::AMP_LABEL_HEIGHT);
    const int bottom_knob_box_size = right_bounds.getWidth() / 3;

    for (size_t i = 4; i <= 6; ++i) // level, mix, master
    {
        AmpKnob knob = current_knobs[i];
        knob.label->setBounds(bottom_label_bounds.removeFromLeft(bottom_knob_box_size)
                                  .withSizeKeepingCentre(
                                      AmpDimensions::AMP_SMALL_KNOB_WIDTH,
                                      AmpDimensions::AMP_LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(right_bounds.removeFromLeft(bottom_knob_box_size)
                                   .withSizeKeepingCentre(
                                       AmpDimensions::AMP_SMALL_KNOB_WIDTH,
                                       AmpDimensions::AMP_SMALL_KNOB_HEIGHT
                                   ));
    }
}

void AmpKnobsComponent::switchColour(juce::Colour colour1, juce::Colour colour2)
{
    juce::ignoreUnused(colour2);
    for (auto knob : current_knobs)
    {
        knob.slider->setColour(juce::Slider::rotarySliderFillColourId, colour1);
    }
    repaint();
}

void AmpKnobsComponent::switchType()
{
    resized();
    removeAllChildren();
    addAndMakeVisible(drag_tooltip);
    slider_being_dragged = false;
    drag_tooltip.setVisible(false);
    for (auto knob : current_knobs)
    {
        addAndMakeVisible(knob.slider);
        addAndMakeVisible(knob.label);
        knob.label->setText(knob.label_text, juce::dontSendNotification);
        knob.label->setJustificationType(juce::Justification::centred);
        knob.slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knob.slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 70, 20);
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

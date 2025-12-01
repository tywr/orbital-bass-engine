#include "amp_knobs_component.h"
#include "../colours.h"
#include "../dimensions.h"

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
    auto bounds = getLocalBounds().reduced(GuiDimensions::PANEL_KNOB_PADDING);

    // Left side: drive knob alone
    auto left_bounds = bounds.removeFromLeft(bounds.getWidth() / 4);
    AmpKnob drive = current_knobs[0]; // drive
    drive.knob->setBounds(left_bounds);

    // Right side: two rows of 3 knobs each
    auto right_bounds = bounds;

    // Top row: era, grunt, attack
    auto top_row_bounds =
        right_bounds.removeFromTop(right_bounds.getHeight() / 2);
    const int top_knob_box_size = top_row_bounds.getWidth() / 3;

    for (size_t i = 1; i <= 3; ++i) // era, grunt, attack
    {
        AmpKnob knob = current_knobs[i];
        knob.knob->setBounds(top_row_bounds.removeFromLeft(top_knob_box_size));
    }

    // Bottom row: level, mix, master
    const int bottom_knob_box_size = right_bounds.getWidth() / 3;

    for (size_t i = 4; i <= 6; ++i) // level, mix, master
    {
        AmpKnob knob = current_knobs[i];
        knob.knob->setBounds(right_bounds.removeFromLeft(bottom_knob_box_size));
    }
}

void AmpKnobsComponent::switchColour(juce::Colour colour1, juce::Colour colour2)
{
    juce::ignoreUnused(colour2);
    for (auto knob : current_knobs)
    {
        knob.knob->getSlider().setColour(
            juce::Slider::rotarySliderFillColourId, colour1
        );
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
        addAndMakeVisible(knob.knob);
        knob.knob->setLabelText(knob.label_text);
        knob.knob->setKnobSize(
            GuiDimensions::KNOB_SIZE, GuiDimensions::KNOB_SIZE
        );
        knob.knob->setLabelHeight(GuiDimensions::KNOB_LABEL_HEIGHT);

        slider_attachments.push_back(
            std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                parameters, knob.parameter_id, knob.knob->getSlider()
            )
        );

        auto& slider = knob.knob->getSlider();
        auto& label = knob.knob->getLabel();
        slider.onDragStart = [this, &slider, &label, labeledKnob = knob.knob]()
        {
            slider_being_dragged = true;
            drag_tooltip.setVisible(false);
            // delay using a Timer
            juce::Timer::callAfterDelay(
                300,
                [this, &slider, &label, labeledKnob]()
                {
                    if (slider.isMouseButtonDown())
                    {
                        drag_tooltip.setText(
                            juce::String(slider.getValue(), 2),
                            juce::dontSendNotification
                        );
                        auto labelBounds = getLocalArea(labeledKnob, label.getBounds());
                        drag_tooltip.setBounds(labelBounds);
                        drag_tooltip.toFront(true);
                        drag_tooltip.setVisible(true);
                        drag_tooltip.repaint();
                    }
                }
            );
        };
        slider.onDragEnd = [this]()
        {
            slider_being_dragged = false;
            drag_tooltip.setVisible(false);
        };
        slider.onValueChange = [this, &slider]()
        {
            if (slider_being_dragged && drag_tooltip.isVisible())
                drag_tooltip.setText(
                    juce::String(slider.getValue(), 2),
                    juce::dontSendNotification
                );
        };
    }
}

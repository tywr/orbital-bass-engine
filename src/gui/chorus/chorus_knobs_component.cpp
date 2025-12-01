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
        addAndMakeVisible(knob.knob);
        knob.knob->setLabelText(knob.label_text);
        knob.knob->setKnobSize(
            ChorusDimensions::KNOB_SIZE, ChorusDimensions::KNOB_SIZE
        );
        knob.knob->setLabelHeight(ChorusDimensions::LABEL_HEIGHT);
        knob.knob->getLabel().setColour(
            juce::Slider::textBoxOutlineColourId,
            juce::Colours::transparentBlack
        );
        knob.knob->getLabel().setColour(
            juce::Slider::textBoxTextColourId, ColourCodes::grey3
        );

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
                        auto labelBounds =
                            getLocalArea(labeledKnob, label.getBounds());
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
    const int top_knob_box_size = top_row_bounds.getWidth() / 2;

    for (size_t i = 0; i < 2; ++i) // rate, depth
    {
        ChorusKnob knob = knobs[i];
        knob.knob->setBounds(top_row_bounds.removeFromLeft(top_knob_box_size));
    }

    // Bottom row: crossover, mix
    const int bottom_knob_box_size = bounds.getWidth() / 2;

    for (size_t i = 2; i < 4; ++i) // crossover, mix
    {
        ChorusKnob knob = knobs[i];
        knob.knob->setBounds(bounds.removeFromLeft(bottom_knob_box_size));
    }
}

void ChorusKnobsComponent::switchColour(
    juce::Colour colour1, juce::Colour colour2
)
{
    juce::ignoreUnused(colour2);
    for (ChorusKnob knob : knobs)
    {
        knob.knob->getSlider().setColour(
            juce::Slider::rotarySliderOutlineColourId, colour1
        );
        knob.knob->getSlider().setColour(
            juce::Slider::rotarySliderFillColourId,
            juce::Colours::transparentBlack
        );
    }
    repaint();
}

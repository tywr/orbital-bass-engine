#include "compressor_knobs_component.h"
#include "../colours.h"
#include "../components/solid_tooltip.h"
#include "compressor_dimensions.h"

CompressorKnobsComponent::CompressorKnobsComponent(
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
            CompressorDimensions::KNOB_SIZE, CompressorDimensions::KNOB_SIZE
        );
        knob.knob->setLabelHeight(CompressorDimensions::LABEL_HEIGHT);
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

CompressorKnobsComponent::~CompressorKnobsComponent()
{
}

void CompressorKnobsComponent::paint(juce::Graphics& g)
{
    juce::ignoreUnused(g);
}

void CompressorKnobsComponent::resized()
{
    auto bounds = getLocalBounds();

    // Left side: threshold alone
    auto left_bounds = bounds.removeFromLeft(bounds.getWidth() / 4);
    CompressorKnob threshold = knobs[1]; // threshold
    threshold.knob->setBounds(left_bounds);

    // Right side: two rows of 3 knobs each
    auto right_bounds = bounds;

    // Top row: hpf, mix, level
    auto top_row_bounds = right_bounds.removeFromTop(right_bounds.getHeight() / 2);
    const int top_knob_box_size = top_row_bounds.getWidth() / 3;

    std::vector<size_t> top_row_indices = {0, 2, 3}; // hpf, mix, level
    for (size_t i : top_row_indices)
    {
        CompressorKnob knob = knobs[i];
        knob.knob->setBounds(top_row_bounds.removeFromLeft(top_knob_box_size));
    }

    // Bottom row: attack, release, ratio
    const int bottom_knob_box_size = right_bounds.getWidth() / 3;

    std::vector<size_t> bottom_row_indices = {5, 6, 4}; // attack, release, ratio
    for (size_t i : bottom_row_indices)
    {
        CompressorKnob knob = knobs[i];
        knob.knob->setBounds(right_bounds.removeFromLeft(bottom_knob_box_size));
    }
}

void CompressorKnobsComponent::switchColour(
    juce::Colour colour1, juce::Colour colour2
)
{
    juce::ignoreUnused(colour2);
    for (CompressorKnob knob : knobs)
    {
        knob.knob->getSlider().setColour(
            juce::Slider::rotarySliderFillColourId, colour1
        );
    }
    repaint();
}

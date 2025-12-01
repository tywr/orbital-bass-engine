#include "eq_sliders_component.h"
#include "../colours.h"
#include "../components/solid_tooltip.h"
#include "../dimensions.h"
#include "eq_dimensions.h"

EqSlidersComponent::EqSlidersComponent(
    juce::AudioProcessorValueTreeState& params
)
    : parameters(params)
{
    for (auto slider : sliders)
    {
        addAndMakeVisible(slider.knob);
        slider.knob->setLabelText(slider.label_text);
        slider.knob->setKnobSize(EqDimensions::KNOB_SIZE, EqDimensions::KNOB_SIZE);
        slider.knob->setLabelHeight(EqDimensions::LABEL_HEIGHT);
        slider.knob->getLabel().setColour(
            juce::Slider::textBoxOutlineColourId,
            juce::Colours::transparentBlack
        );
        slider.knob->getLabel().setColour(
            juce::Slider::textBoxTextColourId, ColourCodes::grey3
        );
        slider_attachments.push_back(
            std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                parameters, slider.parameter_id, slider.knob->getSlider()
            )
        );
    }
}

EqSlidersComponent::~EqSlidersComponent()
{
}

void EqSlidersComponent::paint(juce::Graphics& g)
{
    juce::ignoreUnused(g);
}

void EqSlidersComponent::resized()
{
    auto bounds = getLocalBounds();
    const int section_gap = GuiDimensions::PANEL_GAP * 2; // Extra space between sections

    // Split into two rows
    auto top_row = bounds.removeFromTop(bounds.getHeight() / 2);
    auto bottom_row = bounds;

    // Calculate section widths
    const int single_knob_width = 80;  // Width for single knob sections
    const int double_knob_width = 160; // Width for 2-knob sections

    // TOP ROW: [low_shelf_gain low_shelf_freq] | [low_mid_freq low_mid_gain] | [high_mid_freq high_mid_gain] | [high_shelf_gain high_shelf_freq] | [lpf]

    // Section 1: Low Shelf - gain and freq (knobs 0, 1)
    sliders[0].knob->setBounds(top_row.removeFromLeft(single_knob_width));
    sliders[1].knob->setBounds(top_row.removeFromLeft(single_knob_width));
    top_row.removeFromLeft(section_gap);

    // Section 2: Low-Mid parametric - freq and gain on top row (knobs 2, 4)
    sliders[2].knob->setBounds(top_row.removeFromLeft(single_knob_width));
    sliders[4].knob->setBounds(top_row.removeFromLeft(single_knob_width));
    top_row.removeFromLeft(section_gap);

    // Section 3: High-Mid parametric - freq and gain on top row (knobs 5, 7)
    sliders[5].knob->setBounds(top_row.removeFromLeft(single_knob_width));
    sliders[7].knob->setBounds(top_row.removeFromLeft(single_knob_width));
    top_row.removeFromLeft(section_gap);

    // Section 4: High Shelf - gain and freq (knobs 8, 9)
    sliders[8].knob->setBounds(top_row.removeFromLeft(single_knob_width));
    sliders[9].knob->setBounds(top_row.removeFromLeft(single_knob_width));
    top_row.removeFromLeft(section_gap);

    // Section 5: LPF (knob 10)
    sliders[10].knob->setBounds(top_row.removeFromLeft(single_knob_width));

    // BOTTOM ROW: empty | [low_mid_q centered] | [high_mid_q centered] | empty | empty

    // Skip low shelf section
    bottom_row.removeFromLeft(double_knob_width + section_gap);

    // Low-Mid Q (knob 3) - centered in the low-mid section
    bottom_row.removeFromLeft(single_knob_width / 2 - EqDimensions::KNOB_SIZE / 2);
    sliders[3].knob->setBounds(bottom_row.removeFromLeft(single_knob_width));
    bottom_row.removeFromLeft(single_knob_width / 2 - EqDimensions::KNOB_SIZE / 2 + section_gap);

    // High-Mid Q (knob 6) - centered in the high-mid section
    bottom_row.removeFromLeft(single_knob_width / 2 - EqDimensions::KNOB_SIZE / 2);
    sliders[6].knob->setBounds(bottom_row.removeFromLeft(single_knob_width));
}

void EqSlidersComponent::switchColour(
    juce::Colour colour1, juce::Colour colour2
)
{
    juce::ignoreUnused(colour2);
    for (EqSlider slider : sliders)
    {
        slider.knob->getSlider().setColour(
            juce::Slider::rotarySliderFillColourId, colour1
        );
    }
    repaint();
}

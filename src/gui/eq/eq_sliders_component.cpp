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
        slider.knob->setKnobSize(
            EqDimensions::KNOB_SIZE, EqDimensions::KNOB_SIZE
        );
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
    auto bounds = getLocalBounds();
    const int section_gap = GuiDimensions::PANEL_GAP * 2;

    // Calculate the same layout as resized() to position separators correctly
    const float lpf_width_ratio = 0.15f;
    int lpf_width = bounds.getWidth() * lpf_width_ratio;
    bounds.removeFromRight(lpf_width);
    bounds.removeFromRight(section_gap);

    int total_width = bounds.getWidth();
    const int num_bottom_knobs = 6;
    const int bottom_knob_width = total_width / num_bottom_knobs;

    // Draw vertical separators between EQ sections (SSL-style)
    g.setColour(ColourCodes::grey0);
    float line_thickness = 1.0f;

    // Separator after Low Shelf section (at 1*width/6)
    float x1 = bounds.getX() + 1*bottom_knob_width;
    g.drawLine(x1, bounds.getY(), x1, bounds.getBottom(), line_thickness);

    // Separator after Low-Mid section (at 3*width/6)
    float x2 = bounds.getX() + 3*bottom_knob_width;
    g.drawLine(x2, bounds.getY(), x2, bounds.getBottom(), line_thickness);

    // Separator after High-Mid section (at 5*width/6)
    float x3 = bounds.getX() + 5*bottom_knob_width;
    g.drawLine(x3, bounds.getY(), x3, bounds.getBottom(), line_thickness);
}

void EqSlidersComponent::resized()
{
    auto bounds = getLocalBounds();
    const int section_gap = GuiDimensions::PANEL_GAP * 2;

    // Split horizontally: left side for frequency ranges, right side for LPF
    const float lpf_width_ratio = 0.15f;
    auto lpf_section = bounds.removeFromRight(bounds.getWidth() * lpf_width_ratio);
    bounds.removeFromRight(section_gap);

    int total_width = bounds.getWidth();
    int total_height = bounds.getHeight();

    // Split main section into two rows
    auto top_row = bounds.removeFromTop(total_height / 2);
    auto bottom_row = bounds;

    // BOTTOM ROW: 6 knobs evenly spaced
    const int num_bottom_knobs = 6;
    const int bottom_knob_width = total_width / num_bottom_knobs;

    sliders[1].knob->setBounds(bottom_row.getX() + 0*bottom_knob_width, bottom_row.getY(), bottom_knob_width, bottom_row.getHeight()); // low_shelf_freq
    sliders[2].knob->setBounds(bottom_row.getX() + 1*bottom_knob_width, bottom_row.getY(), bottom_knob_width, bottom_row.getHeight()); // low_mid_freq
    sliders[3].knob->setBounds(bottom_row.getX() + 2*bottom_knob_width, bottom_row.getY(), bottom_knob_width, bottom_row.getHeight()); // low_mid_q
    sliders[5].knob->setBounds(bottom_row.getX() + 3*bottom_knob_width, bottom_row.getY(), bottom_knob_width, bottom_row.getHeight()); // high_mid_freq
    sliders[6].knob->setBounds(bottom_row.getX() + 4*bottom_knob_width, bottom_row.getY(), bottom_knob_width, bottom_row.getHeight()); // high_mid_q
    sliders[9].knob->setBounds(bottom_row.getX() + 5*bottom_knob_width, bottom_row.getY(), bottom_knob_width, bottom_row.getHeight()); // high_shelf_freq

    // TOP ROW: 4 knobs centered over gaps between bottom knobs (skip middle gap)
    // Gap positions are at: 1W/6, 2W/6, skip 3W/6, 4W/6, 5W/6
    const int top_knob_width = bottom_knob_width;

    // Centered over gap 1 (between bottom knobs 0 and 1)
    sliders[0].knob->setBounds(
        top_row.getX() + 1*bottom_knob_width - top_knob_width/2,
        top_row.getY(),
        top_knob_width,
        top_row.getHeight()
    ); // low_shelf_gain

    // Centered over gap 2 (between bottom knobs 1 and 2)
    sliders[4].knob->setBounds(
        top_row.getX() + 2*bottom_knob_width - top_knob_width/2,
        top_row.getY(),
        top_knob_width,
        top_row.getHeight()
    ); // low_mid_gain

    // Skip gap 3 (middle gap)

    // Centered over gap 4 (between bottom knobs 3 and 4)
    sliders[7].knob->setBounds(
        top_row.getX() + 4*bottom_knob_width - top_knob_width/2,
        top_row.getY(),
        top_knob_width,
        top_row.getHeight()
    ); // high_mid_gain

    // Centered over gap 5 (between bottom knobs 4 and 5)
    sliders[8].knob->setBounds(
        top_row.getX() + 5*bottom_knob_width - top_knob_width/2,
        top_row.getY(),
        top_knob_width,
        top_row.getHeight()
    ); // high_shelf_gain

    // LPF section
    sliders[10].knob->setBounds(lpf_section);
}

void EqSlidersComponent::switchColour(
    juce::Colour colour1, juce::Colour colour2
)
{
    juce::ignoreUnused(colour2);
    for (EqSlider slider : sliders)
    {
        slider.knob->getSlider().setColour(
            juce::Slider::rotarySliderOutlineColourId, colour1
        );
        slider.knob->getSlider().setColour(
            juce::Slider::rotarySliderFillColourId,
            juce::Colours::transparentBlack
        );
    }
    repaint();
}

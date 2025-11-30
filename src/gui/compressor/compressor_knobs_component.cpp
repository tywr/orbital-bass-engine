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
        addAndMakeVisible(knob.slider);
        addAndMakeVisible(knob.label);
        knob.label->setText(knob.label_text, juce::dontSendNotification);
        knob.label->setJustificationType(juce::Justification::centred);
        // knob.label->attachToComponent(knob.slider, false);
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
    auto threshold_label_bounds = left_bounds.removeFromTop(CompressorDimensions::LABEL_HEIGHT);
    CompressorKnob threshold_knob = knobs[1]; // threshold
    threshold_knob.label->setBounds(threshold_label_bounds.withSizeKeepingCentre(
        CompressorDimensions::KNOB_SIZE,
        CompressorDimensions::LABEL_HEIGHT
    ));
    threshold_knob.slider->setBounds(left_bounds.withSizeKeepingCentre(
        CompressorDimensions::KNOB_SIZE,
        CompressorDimensions::KNOB_SIZE
    ));

    // Right side: two rows of 3 knobs each
    auto right_bounds = bounds;

    // Top row: hpf, mix, level
    auto top_row_bounds = right_bounds.removeFromTop(right_bounds.getHeight() / 2);
    auto top_label_bounds = top_row_bounds.removeFromTop(CompressorDimensions::LABEL_HEIGHT);
    const int top_knob_box_size = top_row_bounds.getWidth() / 3;

    std::vector<size_t> top_row_indices = {0, 2, 3}; // hpf, mix, level
    for (size_t i : top_row_indices)
    {
        CompressorKnob knob = knobs[i];
        knob.label->setBounds(top_label_bounds.removeFromLeft(top_knob_box_size)
                                  .withSizeKeepingCentre(
                                      CompressorDimensions::KNOB_SIZE,
                                      CompressorDimensions::LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(top_row_bounds.removeFromLeft(top_knob_box_size)
                                   .withSizeKeepingCentre(
                                       CompressorDimensions::KNOB_SIZE,
                                       CompressorDimensions::KNOB_SIZE
                                   ));
    }

    // Bottom row: attack, release, ratio
    auto bottom_label_bounds = right_bounds.removeFromTop(CompressorDimensions::LABEL_HEIGHT);
    const int bottom_knob_box_size = right_bounds.getWidth() / 3;

    std::vector<size_t> bottom_row_indices = {5, 6, 4}; // attack, release, ratio
    for (size_t i : bottom_row_indices)
    {
        CompressorKnob knob = knobs[i];
        knob.label->setBounds(bottom_label_bounds.removeFromLeft(bottom_knob_box_size)
                                  .withSizeKeepingCentre(
                                      CompressorDimensions::KNOB_SIZE,
                                      CompressorDimensions::LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(right_bounds.removeFromLeft(bottom_knob_box_size)
                                   .withSizeKeepingCentre(
                                       CompressorDimensions::KNOB_SIZE,
                                       CompressorDimensions::KNOB_SIZE
                                   ));
    }
}

void CompressorKnobsComponent::switchColour(
    juce::Colour colour1, juce::Colour colour2
)
{
    juce::ignoreUnused(colour2);
    for (CompressorKnob knob : knobs)
    {
        knob.slider->setColour(juce::Slider::rotarySliderFillColourId, colour1);
    }
    repaint();
}

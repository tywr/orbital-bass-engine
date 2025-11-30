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
    int left_size = (int)(0.75f * bounds.getWidth());
    auto left_bounds = bounds.removeFromLeft(left_size).withSizeKeepingCentre(
        left_size, CompressorDimensions::KNOBS_INNER_BOX_HEIGHT
    );
    auto left_label_bounds =
        left_bounds.removeFromTop(CompressorDimensions::LABEL_HEIGHT);
    const int knob_box_size = left_bounds.getWidth() / 4;

    for (size_t i = 0; i < 4; ++i)
    {
        CompressorKnob knob = knobs[i];
        knob.label->setBounds(left_label_bounds.removeFromLeft(knob_box_size)
                                  .withSizeKeepingCentre(
                                      CompressorDimensions::KNOB_SIZE,
                                      CompressorDimensions::LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(left_bounds.removeFromLeft(knob_box_size)
                                   .withSizeKeepingCentre(
                                       CompressorDimensions::KNOB_SIZE,
                                       CompressorDimensions::KNOB_SIZE
                                   ));
    }

    const int knob_box_height = bounds.getHeight() / 3;
    for (size_t i = 4; i < 7; ++i)
    {
        auto knob_bounds = bounds.removeFromTop(knob_box_height);
        CompressorKnob knob = knobs[i];
        knob.label->setBounds(
            knob_bounds.removeFromTop(CompressorDimensions::LABEL_HEIGHT)
                .withSizeKeepingCentre(
                    CompressorDimensions::KNOB_SIZE,
                    CompressorDimensions::LABEL_HEIGHT
                )
        );
        knob.slider->setBounds(knob_bounds.withSizeKeepingCentre(
            CompressorDimensions::SMALL_KNOB_SIZE,
            CompressorDimensions::SMALL_KNOB_SIZE
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

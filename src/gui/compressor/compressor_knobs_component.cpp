#include "compressor_knobs_component.h"
#include "../colours.h"
#include "../components/solid_tooltip.h"
#include "../looks/compressor_look_and_feel.h"
#include "../looks/compressor_selector_look_and_feel.h"
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

    type_slider.setLookAndFeel(&selector_look_and_feel);
    ratio_slider.setLookAndFeel(&selector_look_and_feel);

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
    type_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    ratio_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
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

    int const limit = 3;
    auto bounds = getLocalBounds();
    auto top_bounds =
        bounds.removeFromTop(CompressorDimensions::KNOBS_TOP_BOX_HEIGHT);
    auto label_bounds =
        top_bounds.removeFromTop(CompressorDimensions::LABEL_HEIGHT);
    const int knob_box_size = bounds.getWidth() / limit;

    for (size_t i = 0; i < limit; ++i)
    {
        CompressorKnob knob = knobs[i];
        knob.label->setBounds(label_bounds.removeFromLeft(knob_box_size)
                                  .withSizeKeepingCentre(
                                      CompressorDimensions::KNOB_SIZE,
                                      CompressorDimensions::LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(top_bounds.removeFromLeft(knob_box_size)
                                   .withSizeKeepingCentre(
                                       CompressorDimensions::KNOB_SIZE,
                                       CompressorDimensions::KNOB_SIZE
                                   ));
    }

    auto bottom_knob_box_size = bounds.getWidth() / ((int)knobs.size() - limit);
    auto bottom_bounds =
        bounds.removeFromBottom(CompressorDimensions::KNOBS_BOTTOM_BOX_HEIGHT);
    auto label_bottom_bounds =
        bottom_bounds.removeFromTop(CompressorDimensions::LABEL_HEIGHT);

    for (size_t i = limit; i < knobs.size(); ++i)
    {
        CompressorKnob knob = knobs[i];
        knob.label->setBounds(label_bottom_bounds
                                  .removeFromLeft(bottom_knob_box_size)
                                  .withSizeKeepingCentre(
                                      CompressorDimensions::KNOB_SIZE,
                                      CompressorDimensions::LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(bottom_bounds
                                   .removeFromLeft(bottom_knob_box_size)
                                   .withSizeKeepingCentre(
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

#include "amp_knobs_component.h"
#include "../colours.h"
#include "../looks/amp_small_look_and_feel.h"
#include "amp_dimensions.h"

AmpKnobsComponent::AmpKnobsComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params)
{
    setLookAndFeel(new AmpSmallLookAndFeel());
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
    auto label_bounds = bounds.removeFromTop(AmpDimensions::AMP_LABEL_HEIGHT);
    const int knob_box_size =
        bounds.getWidth() / static_cast<int>(current_knobs.size());

    for (auto knob : current_knobs)
    {
        knob.label->setBounds(label_bounds.removeFromLeft(knob_box_size)
                                  .withSizeKeepingCentre(
                                      AmpDimensions::AMP_SMALL_KNOB_WIDTH,
                                      AmpDimensions::AMP_LABEL_HEIGHT
                                  ));
        knob.slider->setBounds(bounds.removeFromLeft(knob_box_size)
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

void AmpKnobsComponent::switchType(AmpType new_type)
{
    // clear tooltip before switching
    slider_being_dragged = false;
    drag_tooltip.setVisible(false);
    if (new_type.id == "helios")
    {
        current_knobs[1] = {
            &grunt_slider, &grunt_label, "overdrive_grunt", "grunt"
        };
        current_knobs[2] = {
            &attack_slider, &attack_label, "overdrive_attack", "attack"
        };
    }
    if (new_type.id == "borealis")
    {
        current_knobs[1] = {
            &cross_frequency_slider, &cross_frequency_label, "overdrive_x",
            "hi pass"
        };
        current_knobs[2] = {
            &high_level_slider, &high_level_label, "overdrive_x_level",
            "hi level"
        };
    }
    if (new_type.id == "nebula")
    {
        current_knobs[1] = {&mod_slider, &mod_label, "overdrive_mod", "mod"};
        current_knobs[2] = {
            &aggro_slider, &aggro_label, "overdrive_aggro", "aggro"
        };
    }
    resized();
    removeAllChildren();
    addAndMakeVisible(drag_tooltip);
    for (auto knob : current_knobs)
    {
        addAndMakeVisible(knob.slider);
        addAndMakeVisible(knob.label);
        knob.label->setText(knob.label_text, juce::dontSendNotification);
        knob.label->setJustificationType(juce::Justification::centred);
        // knob.label->attachToComponent(knob.slider, false);
        knob.slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knob.slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 70, 20);
        // knob.slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70,
        // 20); knob.label->setColour(
        //     juce::Slider::textBoxOutlineColourId,
        //     juce::Colours::transparentBlack
        // );
        // knob.label->setColour(
        //     juce::Slider::textBoxTextColourId, ColourCodes::grey3
        // );
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

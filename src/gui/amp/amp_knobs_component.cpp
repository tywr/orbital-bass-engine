#include "amp_knobs_component.h"
#include "../colours.h"
#include "../looks/amp_small_look_and_feel.h"
#include "amp_dimensions.h"

AmpKnobsComponent::AmpKnobsComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params)
{
    setLookAndFeel(new AmpSmallLookAndFeel());
}

AmpKnobsComponent::~AmpKnobsComponent()
{
}

void AmpKnobsComponent::paint(juce::Graphics& g)
{
}

void AmpKnobsComponent::resized()
{

    auto bounds = getLocalBounds();
    auto label_bounds = bounds.removeFromTop(AmpDimensions::AMP_LABEL_HEIGHT);
    const int knob_box_size = bounds.getWidth() / current_knobs.size();

    for (auto knob : current_knobs)
    {
        knob.label->setBounds(label_bounds.removeFromLeft(knob_box_size)
                                  .withSizeKeepingCentre(
                                      AmpDimensions::AMP_SMALL_KNOB_WIDTH,
                                      AmpDimensions::AMP_SMALL_LABEL_HEIGHT
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
    if (new_type.id == "helios")
    {
        current_knobs[1] = {
            &grunt_slider, &grunt_label, "overdrive_grunt", "GRUNT"
        };
        current_knobs[2] = {
            &attack_slider, &attack_label, "overdrive_attack", "ATTACK"
        };
    }
    if (new_type.id == "borealis")
    {
        current_knobs[1] = {
            &cross_frequency_slider, &cross_frequency_label, "overdrive_x", "X"
        };
        current_knobs[2] = {
            &high_level_slider, &high_level_label, "overdrive_x_level",
            "X-LEVEL"
        };
    }
    if (new_type.id == "nebula")
    {
        current_knobs[1] = {&mod_slider, &mod_label, "overdrive_mod", "MOD"};
        current_knobs[2] = {
            &aggro_slider, &aggro_label, "overdrive_aggro", "AGGRO"
        };
    }
    resized();
    removeAllChildren();
    for (auto knob : current_knobs)
    {
        addAndMakeVisible(knob.slider);
        addAndMakeVisible(knob.label);
        knob.label->setText(knob.label_text, juce::dontSendNotification);
        knob.label->setJustificationType(juce::Justification::centred);
        // knob.label->attachToComponent(knob.slider, false);
        knob.slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knob.slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
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
    }
}

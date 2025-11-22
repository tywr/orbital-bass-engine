#include "eq_component.h"
#include "../colours.h"
#include "../looks/eq_look_and_feel.h"
#include "eq_dimensions.h"
#include "eq_sliders_component.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

EqComponent::EqComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params), sliders_component(params)
{
    setLookAndFeel(new EqLookAndFeel());
    addAndMakeVisible(sliders_component);
    addAndMakeVisible(bypass_button);
    // addAndMakeVisible(lpf_slider);

    // lpf_slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    // lpf_slider.setTextBoxStyle(
    //     juce::Slider::TextBoxBelow, false, 70, EqDimensions::LABEL_HEIGHT
    // );
    // lpf_slider_attachment =
    //     std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
    //         parameters, "eq_lpf", lpf_slider
    //     );

    bypass_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "eq_bypass", bypass_button
        );

    bypass_button.setColour(
        juce::ToggleButton::tickColourId, GuiColours::DEFAULT_INACTIVE_COLOUR
    );
    bypass_button.setColour(
        juce::ToggleButton::tickDisabledColourId, GuiColours::EQ_ACTIVE_COLOUR_1
    );
    bypass_button.onClick = [this]() { repaint(); };
}

EqComponent::~EqComponent()
{
}

void EqComponent::paint(juce::Graphics& g)
{
    bool bypass = bypass_button.getToggleState();
    juce::Colour colour1;
    juce::Colour colour2;
    if (!bypass)
    {
        colour1 = GuiColours::EQ_ACTIVE_COLOUR_1;
        colour2 = GuiColours::EQ_ACTIVE_COLOUR_2;
    }
    else
    {
        colour1 = GuiColours::DEFAULT_INACTIVE_COLOUR;
        colour2 = GuiColours::DEFAULT_INACTIVE_COLOUR;
    }
    float border_thickness = EqDimensions::BORDER_THICKNESS;
    float border_radius = EqDimensions::CORNER_RADIUS;

    auto outer_bounds =
        getLocalBounds()
            .withSizeKeepingCentre(EqDimensions::WIDTH, EqDimensions::HEIGHT)
            .toFloat();
    auto inner_bounds = outer_bounds.reduced(border_thickness).toFloat();

    g.setColour(GuiColours::EQ_BG_COLOUR);
    g.fillRoundedRectangle(inner_bounds, border_radius);

    juce::Path border_path;
    border_path.addRoundedRectangle(
        outer_bounds, border_radius + border_thickness
    );
    border_path.addRoundedRectangle(inner_bounds, border_radius);
    border_path.setUsingNonZeroWinding(false);

    juce::ColourGradient border_gradient(
        colour1, outer_bounds.getTopLeft(), colour2,
        outer_bounds.getBottomLeft(), false
    );
    g.setGradientFill(border_gradient);
    g.fillPath(border_path);

    sliders_component.switchColour(colour1, colour2);

    // lpf_slider.setColour(juce::Slider::rotarySliderFillColourId, colour1);
}

void EqComponent::resized()
{
    auto bounds = getLocalBounds().withSizeKeepingCentre(
        EqDimensions::WIDTH, EqDimensions::HEIGHT
    );
    auto middle_bounds = bounds.withSizeKeepingCentre(
        bounds.getWidth() - EqDimensions::SIDE_WIDTH * 2,
        EqDimensions::BOX_HEIGHT
    );
    sliders_component.setBounds(middle_bounds);

    auto left_bounds = bounds.removeFromLeft(EqDimensions::SIDE_WIDTH);
    bypass_button.setBounds(left_bounds.withSizeKeepingCentre(
        EqDimensions::BYPASS_SIZE, EqDimensions::BYPASS_SIZE
    ));

    // auto right_bounds = bounds.removeFromRight(EqDimensions::SIDE_WIDTH);
    // lpf_slider.setBounds(right_bounds.withSizeKeepingCentre(
    //     EqDimensions::KNOB_SIZE,
    //     EqDimensions::KNOB_SIZE + EqDimensions::LABEL_HEIGHT
    // ));
}

#include "amp_component.h"
#include "../colours.h"
#include "amp_dimensions.h"
#include "amp_knobs_component.h"
#include "designs/borealis.h"
#include "designs/helios.h"
#include "designs/nebula.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

AmpComponent::AmpComponent(juce::AudioProcessorValueTreeState& params)
    : parameters(params), knobs_component(params)
{

    addAndMakeVisible(knobs_component);
    addAndMakeVisible(bypass_button);

    bypass_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            parameters, "amp_bypass", bypass_button
        );
    bypass_button.onClick = [this]() { repaint(); };

    type_slider_attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            parameters, "amp_type", type_slider
        );

    for (auto type : types)
    {
        addAndMakeVisible(type.button);
        type.button->onClick = [this, type]() { switchType(type); };
    }
    initType();
    knobs_component.switchType(selected_type);
}

AmpComponent::~AmpComponent()
{
}

void AmpComponent::initType()
{
    int current_index = static_cast<int>(type_slider.getValue());
    auto current_type = types[current_index];
    for (auto type : types)
    {
        if (type.id == current_type.id)
        {
            selected_type = current_type;
            type.button->setToggleState(true, juce::dontSendNotification);
        }
        else
        {
            type.button->setToggleState(false, juce::dontSendNotification);
        }
    }
}

void AmpComponent::switchType(AmpType new_type)
{
    selected_type = new_type;
    knobs_component.switchType(new_type);
    double index = 0;
    for (auto type : types)
    {
        if (type.id == new_type.id)
        {
            type_slider.setValue(index);
            type.button->setToggleState(true, juce::dontSendNotification);
            type.button->setEnabled(false);
        }
        else
        {
            type.button->setToggleState(false, juce::dontSendNotification);
            type.button->setEnabled(true);
        }
        index += 1;
    }
    repaint();
}

void AmpComponent::paintTypeButtons(juce::Graphics& g)
{
}

void AmpComponent::paintDesign(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    if (selected_type.id == "helios")
    {
        paintDesignHelios(g, bounds, current_colour1, current_colour2);
    }
    else if (selected_type.id == "borealis")
    {
        paintDesignBorealis(g, bounds, current_colour1, current_colour2);
    }
    else if (selected_type.id == "nebula")
    {
        paintDesignNebula(g, bounds, current_colour1, current_colour2);
    }
}

void AmpComponent::paintBorder(
    juce::Graphics& g, juce::Rectangle<float> bounds, float border_radius
)
{
    float border_thickness = AmpDimensions::AMP_BORDER_THICKNESS;

    auto outer_bounds = bounds.toFloat();
    auto inner_bounds = bounds.reduced(border_thickness).toFloat();

    g.setColour(GuiColours::COMPRESSOR_BG_COLOUR);
    g.fillRoundedRectangle(inner_bounds, border_radius);

    juce::Path border_path;
    border_path.addRoundedRectangle(
        outer_bounds, border_radius + border_thickness
    );
    border_path.addRoundedRectangle(inner_bounds, border_radius);
    border_path.setUsingNonZeroWinding(false);

    juce::ColourGradient border_gradient(
        current_colour1, outer_bounds.getTopLeft(), current_colour2,
        outer_bounds.getBottomLeft(), false
    );
    g.setGradientFill(border_gradient);
    g.fillPath(border_path);
}

void AmpComponent::paint(juce::Graphics& g)
{
    paintTypeButtons(g);

    bool bypass = bypass_button.getToggleState();

    if (!bypass)
    {
        current_colour1 = selected_type.colour1;
        current_colour2 = selected_type.colour2;
    }
    else
    {
        current_colour1 = GuiColours::DEFAULT_INACTIVE_COLOUR;
        current_colour2 = GuiColours::DEFAULT_INACTIVE_COLOUR;
    }
    bypass_button.setColour(
        juce::ToggleButton::tickColourId, GuiColours::DEFAULT_INACTIVE_COLOUR
    );
    bypass_button.setColour(
        juce::ToggleButton::tickDisabledColourId, current_colour1
    );

    auto outer_bounds =
        getLocalBounds()
            .withTrimmedTop(AmpDimensions::AMP_TYPE_BUTTONS_HEIGHT)
            .withSizeKeepingCentre(
                AmpDimensions::AMP_WIDTH, AmpDimensions::AMP_HEIGHT
            )
            .toFloat();

    // paintBorder(g, outer_bounds, AmpDimensions::AMP_BORDER_RADIUS);

    auto frame_bounds = outer_bounds.reduced(AmpDimensions::AMP_FRAME_PADDING);
    auto higher_frame_bounds = frame_bounds.withTrimmedBottom(
        AmpDimensions::AMP_KNOBS_BOTTOM_BOX_HEIGHT +
        AmpDimensions::AMP_INNER_TOP_PADDING
    );
    auto lower_frame_bounds =
        frame_bounds.withTrimmedTop(higher_frame_bounds.getHeight());

    g.setColour(GuiColours::AMP_BG_COLOUR);
    g.fillRect(frame_bounds);

    g.setColour(GuiColours::DEFAULT_INACTIVE_COLOUR);
    g.drawLine(
        lower_frame_bounds.getX(), lower_frame_bounds.getY(),
        lower_frame_bounds.getRight(), lower_frame_bounds.getY(), 1.0f
    );

    paintDesign(g, higher_frame_bounds);

    knobs_component.switchColour(current_colour1, current_colour2);

    juce::Path path;
    path.addRoundedRectangle(outer_bounds, AmpDimensions::AMP_BORDER_RADIUS);
    path.addRoundedRectangle(
        frame_bounds,
        AmpDimensions::AMP_BORDER_RADIUS - AmpDimensions::AMP_FRAME_PADDING
    );
    path.setUsingNonZeroWinding(false);
    juce::ColourGradient gradient(
        current_colour1, outer_bounds.getX(),
        outer_bounds.getY(), // start color and point
        current_colour2, outer_bounds.getX(),
        outer_bounds.getBottom(), // end color and point
        false                     // not radial
    );
    g.setGradientFill(gradient);
    g.fillPath(path);
}

void AmpComponent::resized()
{
    auto bounds = getLocalBounds();

    // Type buttons at the top
    auto type_bounds =
        bounds.removeFromTop(AmpDimensions::AMP_TYPE_BUTTONS_HEIGHT);
    int const type_size = type_bounds.getWidth() / types.size();
    for (auto type : types)
    {
        type.button->setBounds(
            type_bounds.removeFromLeft(type_size).withSizeKeepingCentre(
                AmpDimensions::AMP_TYPE_BUTTON_SIZE,
                AmpDimensions::AMP_TYPE_BUTTON_SIZE
            )
        );
    }

    // Amp body
    auto amp_bounds = bounds.withSizeKeepingCentre(
        AmpDimensions::AMP_WIDTH, AmpDimensions::AMP_HEIGHT
    );

    // Bottom part with buttons
    auto bottom_bounds =
        amp_bounds
            .reduced(
                0, AmpDimensions::AMP_INNER_BOTTOM_PADDING +
                       AmpDimensions::AMP_FRAME_PADDING
            )
            .removeFromBottom(AmpDimensions::AMP_KNOBS_BOTTOM_BOX_HEIGHT);
    knobs_component.setBounds(
        bottom_bounds.reduced(AmpDimensions::AMP_SIDE_WIDTH, 0)
    );
    bypass_button.setBounds(
        bottom_bounds.removeFromLeft(AmpDimensions::AMP_SIDE_WIDTH)
            .withSizeKeepingCentre(
                AmpDimensions::AMP_BYPASS_SIDE, AmpDimensions::AMP_BYPASS_SIDE
            )
    );
}

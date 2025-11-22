#include "post_rack.h"
#include "colours.h"
#include <juce_audio_basics/juce_audio_basics.h>

PostRackComponent::PostRackComponent(juce::AudioProcessorValueTreeState& params)
    : chorus_component(params), ir_component(params), eq_component(params)
{
    addAndMakeVisible(chorus_component);
    addAndMakeVisible(ir_component);
    addAndMakeVisible(eq_component);
}

PostRackComponent::~PostRackComponent()
{
}

void PostRackComponent::paint(juce::Graphics& g)
{
    const float borderThickness = 6.0f;
    const float borderRadius = 20.0f;
    const int totalWidth = 850;
    const int totalHeight = 150 + 100 + 100;

    auto outer_bounds = getLocalBounds()
                            .withSizeKeepingCentre(totalWidth, totalHeight)
                            .toFloat();
    auto inner_bounds = outer_bounds.reduced(borderThickness).toFloat();

    // Draw background
    g.setColour(ColourCodes::bg0);
    g.fillRoundedRectangle(inner_bounds, borderRadius);

    // Draw outer border
    juce::Path border_path;
    border_path.addRoundedRectangle(
        outer_bounds, borderRadius + borderThickness
    );
    border_path.addRoundedRectangle(inner_bounds, borderRadius);
    border_path.setUsingNonZeroWinding(false);

    g.setColour(GuiColours::RACK_COLOUR);
    g.fillPath(border_path);

    // Draw separator lines between components
    float separator1Y = inner_bounds.getY() + 150;
    g.setColour(GuiColours::DEFAULT_INACTIVE_COLOUR);
    g.drawLine(
        inner_bounds.getX(), separator1Y, inner_bounds.getRight(), separator1Y,
        1.0f
    );

    // Second separator: between Chorus and IR (at y = 150 + 100)
    float separator2Y = inner_bounds.getY() + 150 + 100;
    g.drawLine(
        inner_bounds.getX(), separator2Y, inner_bounds.getRight(), separator2Y,
        1.0f
    );
}

void PostRackComponent::resized()
{
    auto bounds = getLocalBounds().withSizeKeepingCentre(850, 150 + 100 + 100);
    eq_component.setBounds(bounds.removeFromTop(150));
    chorus_component.setBounds(bounds.removeFromTop(100));
    ir_component.setBounds(bounds.removeFromBottom(100));
}

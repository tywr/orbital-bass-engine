#include "post_rack.h"
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

void PostRackComponent::resized()
{
    auto bounds = getLocalBounds().withSizeKeepingCentre(
        850, 180 + 100 + 100 + 16
    );
    eq_component.setBounds(bounds.removeFromTop(180));
    bounds.removeFromTop(8);
    chorus_component.setBounds(bounds.removeFromTop(100));
    bounds.removeFromTop(8);
    ir_component.setBounds(bounds.removeFromBottom(100));
}

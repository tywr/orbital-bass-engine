#include "post_rack.h"
#include <juce_audio_basics/juce_audio_basics.h>

PostRackComponent::PostRackComponent(juce::AudioProcessorValueTreeState& params)
    : chorus_component(params), ir_component(params)
{
    addAndMakeVisible(chorus_component);
    addAndMakeVisible(ir_component);
}

PostRackComponent::~PostRackComponent()
{
}

void PostRackComponent::resized()
{
    int size = 2;
    auto bounds = getLocalBounds().withSizeKeepingCentre(
        850, 120 * size + 10 * (size - 1)
    );
    chorus_component.setBounds(bounds.removeFromTop(120));
    ir_component.setBounds(bounds.removeFromBottom(120));
}

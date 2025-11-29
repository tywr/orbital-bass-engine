#include "panels.h"

Panels::Panels(
    juce::AudioProcessorValueTreeState& params,
    juce::Value& compressorGainReductionDb
)
    : compressor_component(params, compressorGainReductionDb),
      amp_component(params), post_rack_component(params)
{
    addAndMakeVisible(compressor_component);
    addAndMakeVisible(amp_component);
    addAndMakeVisible(post_rack_component);
}

Panels::~Panels()
{
}

void Panels::paint(juce::Graphics& g)
{
    juce::ignoreUnused(g);
}

void Panels::resized()
{
    auto bounds = getLocalBounds();

    // Top section split: compressor on left, amp on right
    auto top_section = bounds.removeFromTop(2 * bounds.getHeight() / 3);
    int top_half_width = top_section.getWidth() / 2;

    compressor_component.setBounds(top_section.removeFromLeft(top_half_width));
    amp_component.setBounds(top_section);

    // Bottom section: post effects takes full width
    post_rack_component.setBounds(bounds);
}

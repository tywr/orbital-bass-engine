#include "panels.h"
#include "dimensions.h"

Panels::Panels(
    juce::AudioProcessorValueTreeState& params,
    juce::Value& compressorGainReductionDb
)
    : compressor_component(params, compressorGainReductionDb),
      amp_component(params), eq_component(params), chorus_component(params),
      ir_component(params)
{
    addAndMakeVisible(compressor_component);
    addAndMakeVisible(amp_component);
    addAndMakeVisible(eq_component);
    addAndMakeVisible(chorus_component);
    addAndMakeVisible(ir_component);
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
    auto bounds = getLocalBounds().reduced(GuiDimensions::PANEL_GAP);

    // Top section split: compressor on left, amp on right
    auto top_section = bounds.removeFromTop(2 * bounds.getHeight() / 3);
    int top_half_width = top_section.getWidth() / 2;

    compressor_component.setBounds(top_section.removeFromLeft(top_half_width - GuiDimensions::PANEL_GAP / 2));
    top_section.removeFromLeft(GuiDimensions::PANEL_GAP);
    amp_component.setBounds(top_section);

    // Gap between top and bottom sections
    bounds.removeFromTop(GuiDimensions::PANEL_GAP);

    // Bottom section: three panels (EQ, Chorus, IR) split horizontally
    int panel_width = bounds.getWidth();
    eq_component.setBounds(bounds.removeFromLeft(panel_width * 0.7f - GuiDimensions::PANEL_GAP));
    bounds.removeFromLeft(GuiDimensions::PANEL_GAP);
    chorus_component.setBounds(bounds.removeFromLeft(panel_width * 0.15f - GuiDimensions::PANEL_GAP / 2));
    bounds.removeFromLeft(GuiDimensions::PANEL_GAP);
    ir_component.setBounds(bounds);
}

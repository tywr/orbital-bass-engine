#include "tabs.h"
#include "amp/amp_component.h"
#include "chorus/chorus_component.h"
#include "colours.h"
#include "compressor/compressor_component.h"
#include "ir/ir_component.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

Tabs::Tabs(
    juce::AudioProcessorValueTreeState& params,
    juce::Value& compressorGainReductionDb
)
    : juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop),
      parameters(params),
      compressor_component(params, compressorGainReductionDb),
      amp_component(params), chorus_component(params), ir_component(params)
{
    setColour(
        juce::TabbedComponent::backgroundColourId,
        juce::Colours::transparentBlack
    );
    setColour(
        juce::TabbedComponent::outlineColourId, juce::Colours::transparentBlack
    );

    addTab("comp", ColourCodes::bg, &compressor_component, true);
    addTab("amp", ColourCodes::bg, &amp_component, true);
    addTab("chorus", ColourCodes::bg, &chorus_component, true);
    addTab("cabinet", ColourCodes::bg, &ir_component, true);
    setTabBarDepth(60);
}

Tabs::~Tabs()
{
}

void Tabs::paint(juce::Graphics& g)
{
    juce::ignoreUnused(g);
}

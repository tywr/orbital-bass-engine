#include "tabs.h"
#include "amp/amp_component.h"
#include "chorus/chorus_component.h"
#include "colours.h"
#include "compressor/compressor_component.h"
#include "fuzz/fuzz_component.h"
#include "ir/ir_component.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

Tabs::Tabs(
    juce::AudioProcessorValueTreeState& params,
    juce::Value& compressorGainReductionDb
)
    : juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop),
      compressor_component(params, compressorGainReductionDb),
      fuzz_component(params), amp_component(params), chorus_component(params),
      ir_component(params), post_rack_component(params)
{
    setColour(
        juce::TabbedComponent::backgroundColourId,
        juce::Colours::transparentBlack
    );
    setColour(
        juce::TabbedComponent::outlineColourId, juce::Colours::transparentBlack
    );

    addTab("comp", ColourCodes::bg, &compressor_component, true);
    // addTab("fuzz", ColourCodes::bg, &fuzz_component, true);
    addTab("amp", ColourCodes::bg, &amp_component, true);
    addTab("post", ColourCodes::bg, &post_rack_component, true);
    setTabBarDepth(60);
}

Tabs::~Tabs()
{
}

void Tabs::paint(juce::Graphics& g)
{
    juce::ignoreUnused(g);
}

#include "tabs.h"
#include "amp/amp_component.h"
#include "colours.h"
#include "compressor/compressor_component.h"
#include "ir_gui.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

Tabs::Tabs(
    juce::AudioProcessorValueTreeState& params,
    juce::Value& compressorGainReductionDb
)
    : juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop),
      parameters(params),
      compressor_component(params, compressorGainReductionDb),
      amp_component(params)
{
    setColour(
        juce::TabbedComponent::backgroundColourId,
        juce::Colours::transparentBlack
    );
    setColour(
        juce::TabbedComponent::outlineColourId, juce::Colours::transparentBlack
    );

    // addTab("COMP", ColourCodes::bg, &compressor_component, false);
    addTab("AMP", ColourCodes::bg, &amp_component, true);
    // addTab("CHORUS", AuroraColors::bg, new juce::Component(), true);
    // addTab("IR", ColourCodes::bg, new IRLoader(params), true);
    setTabBarDepth(60);
}

Tabs::~Tabs()
{
}

void Tabs::paint(juce::Graphics& g)
{
    juce::ignoreUnused(g);
}

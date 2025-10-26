#include "plugin_editor.h"

#include "gui/looks/base_look_and_feel.h"
#include "plugin_audio_processor.h"
#include <juce_core/juce_core.h>

using namespace juce;

//==============================================================================
PluginEditor::PluginEditor(
    PluginAudioProcessor& p, juce::AudioProcessorValueTreeState& params
)
    : AudioProcessorEditor(&p), processorRef(p), parameters(params),
      header(params, processorRef.inputLevel, processorRef.outputLevel),
      tabs(params, processorRef.compressorGainReductionDb)
{

    setLookAndFeel(new BaseLookAndFeel());
    setSize(900, 650);
    addAndMakeVisible(header);
    addAndMakeVisible(tabs);

    juce::ignoreUnused(processorRef);

    getLookAndFeel().setColour(
        juce::Slider::thumbColourId, juce::Colours::lightgreen
    );
}

PluginEditor::~PluginEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void PluginEditor::paint(juce::Graphics& g)
{
    float scale = g.getInternalContext().getPhysicalPixelScaleFactor();
    if (!is_background_drawn)
    {
        paintBackground(scale);
        is_background_drawn = true;
    }
    auto bounds = getLocalBounds();
    g.drawImageAt(background, bounds.getX(), bounds.getY());
}

void PluginEditor::paintBackground(float scale)
{
    int width = static_cast<int>(scale * getWidth());
    int height = static_cast<int>(scale * getHeight());
    background = juce::Image(juce::Image::ARGB, width, height, true);

    juce::Graphics g(background);
    juce::Graphics cache(background);
    g.fillAll(juce::Colours::black);

    juce::Random random(3);
    const int gridSize = 30; // Space between potential dots

    for (int x = 0; x < getWidth(); x += gridSize)
    {
        for (int y = 0; y < getHeight(); y += gridSize)
        {
            if (random.nextFloat() > 0.7f) // 30% chance of dot
            {
                float offsetX = random.nextFloat() * gridSize;
                float offsetY = random.nextFloat() * gridSize;
                float size = random.nextFloat() * 3.0f + 0.5f;
                float alpha = random.nextFloat() * 0.5f + 0.05f;

                g.setColour(juce::Colours::white.withAlpha(alpha));
                g.fillEllipse(x + offsetX, y + offsetY, size, size);
            }
        }
    }
}

void PluginEditor::resized()
{
    const float header_ratio = 0.1f;
    const int header_height = static_cast<int>(getHeight() * header_ratio);
    auto bounds = getLocalBounds();
    header.setBounds(bounds.removeFromTop(header_height));
    tabs.setBounds(bounds);
}

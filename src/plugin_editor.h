#pragma once

#include "gui/header.h"
#include "gui/tabs.h"
#include "gui/tuner.h"
#include "plugin_audio_processor.h"

//==============================================================================
class PluginEditor final : public juce::AudioProcessorEditor
{
  public:
    explicit PluginEditor(
        PluginAudioProcessor&, juce::AudioProcessorValueTreeState&
    );
    ~PluginEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void paintBackground(float scale);
    void resized() override;

    void setupGainControl(
        juce::Slider& slider, juce::Label& label, double minRange,
        double maxRange, double initialValue, const juce::String& labelText
    );
    void setupGainMeter(juce::Slider& slider, double minRange, double maxRange);
    void resizeGainControls();

  private:
    juce::Image background;
    bool is_background_drawn = false;
    PluginAudioProcessor& processorRef;
    juce::AudioProcessorValueTreeState& parameters;
    Header header;
    Tabs tabs;
    Tuner tuner;

    void showTuner();
    void hideTuner();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};

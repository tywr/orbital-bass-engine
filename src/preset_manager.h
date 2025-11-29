#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

struct Preset {
    juce::String name;
    juce::ValueTree state;
    bool isEmpty = true;

    Preset() = default;
    Preset(const juce::String& presetName, const juce::ValueTree& presetState)
        : name(presetName), state(presetState), isEmpty(false) {}
};

class PresetManager {
public:
    PresetManager(juce::AudioProcessorValueTreeState& apvts);

    bool savePreset(const juce::File& file, const juce::String& presetName);

    bool loadPreset(const juce::File& file, Preset& outPreset);

    void applyPreset(const Preset& preset);

    Preset getCurrentStateAsPreset(const juce::String& name);

private:
    juce::AudioProcessorValueTreeState& parameters;
};

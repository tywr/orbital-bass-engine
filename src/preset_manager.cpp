#include "preset_manager.h"

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& apvts)
    : parameters(apvts)
{
}

bool PresetManager::savePreset(const juce::File& file, const juce::String& presetName)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    if (xml != nullptr)
    {
        xml->setAttribute("presetName", presetName);
        return xml->writeTo(file);
    }

    return false;
}

bool PresetManager::loadPreset(const juce::File& file, Preset& outPreset)
{
    if (!file.existsAsFile())
        return false;

    std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument::parse(file));

    if (xml == nullptr)
        return false;

    juce::String presetName = xml->getStringAttribute("presetName", file.getFileNameWithoutExtension());

    juce::ValueTree state = juce::ValueTree::fromXml(*xml);

    if (!state.isValid())
        return false;

    outPreset.name = presetName;
    outPreset.state = state;
    outPreset.isEmpty = false;

    return true;
}

void PresetManager::applyPreset(const Preset& preset)
{
    if (preset.isEmpty)
        return;

    parameters.replaceState(preset.state);
}

Preset PresetManager::getCurrentStateAsPreset(const juce::String& name)
{
    auto state = parameters.copyState();
    return Preset(name, state);
}

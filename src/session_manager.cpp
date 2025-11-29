#include "session_manager.h"

SessionManager::SessionManager(PresetManager& pm)
    : presetManager(pm)
{
}

bool SessionManager::loadSessionFromFolder(const juce::File& folder)
{
    if (!folder.isDirectory())
        return false;

    clearSession();

    sessionName = folder.getFileName();
    sessionFolder = folder;

    int loadedCount = 0;
    for (int i = 0; i < MAX_PRESETS; ++i)
    {
        juce::File presetFile = folder.getChildFile("preset_" + juce::String(i + 1) + ".xml");
        if (presetFile.existsAsFile())
        {
            Preset preset;
            if (presetManager.loadPreset(presetFile, preset))
            {
                presets[i] = preset;
                loadedCount++;
            }
        }
    }

    notifySessionChanged();

    return true;
}

void SessionManager::saveSession(const juce::File& folder, const juce::String& name)
{
    if (!folder.exists())
        folder.createDirectory();

    sessionName = name;
    sessionFolder = folder;

    for (int i = 0; i < MAX_PRESETS; ++i)
    {
        if (!presets[i].isEmpty)
        {
            juce::File presetFile = folder.getChildFile("preset_" + juce::String(i + 1) + ".xml");
            presetManager.savePreset(presetFile, presets[i].name);
            presets[i].sourceFile = presetFile;
        }
    }
}

const Preset& SessionManager::getPreset(int index) const
{
    jassert(index >= 0 && index < MAX_PRESETS);
    return presets[index];
}

void SessionManager::setPreset(int index, const Preset& preset)
{
    jassert(index >= 0 && index < MAX_PRESETS);

    if (sessionFolder != juce::File() && !preset.isEmpty)
    {
        juce::File presetFile = sessionFolder.getChildFile("preset_" + juce::String(index + 1) + ".xml");
        presetManager.savePreset(presetFile, preset.name);

        presets[index] = preset;
        presets[index].sourceFile = presetFile;
    }
    else
    {
        presets[index] = preset;
    }

    notifySessionChanged();
}

void SessionManager::clearPreset(int index)
{
    jassert(index >= 0 && index < MAX_PRESETS);

    if (sessionFolder != juce::File())
    {
        const auto& preset = presets[index];
        if (!preset.isEmpty && preset.sourceFile.existsAsFile())
        {
            preset.sourceFile.deleteFile();
        }
    }

    presets[index] = Preset();
    notifySessionChanged();
}

void SessionManager::clearSession()
{
    for (auto& preset : presets)
        preset = Preset();

    sessionName.clear();
    sessionFolder = juce::File();
    currentPresetIndex = -1;
    notifySessionChanged();
}

void SessionManager::setCurrentPresetIndex(int index)
{
    if (index >= -1 && index < MAX_PRESETS)
    {
        currentPresetIndex = index;
        notifyCurrentPresetChanged();
    }
}

void SessionManager::addListener(Listener* listener)
{
    listeners.add(listener);
}

void SessionManager::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

void SessionManager::notifySessionChanged()
{
    listeners.call(&Listener::sessionChanged);
}

void SessionManager::notifyCurrentPresetChanged()
{
    listeners.call(&Listener::currentPresetChanged, currentPresetIndex);
}

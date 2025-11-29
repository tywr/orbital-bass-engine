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

    auto xmlFiles = folder.findChildFiles(juce::File::findFiles, false, "*.xml");

    int loadedCount = 0;
    for (int i = 0; i < juce::jmin(xmlFiles.size(), MAX_PRESETS); ++i)
    {
        Preset preset;
        if (presetManager.loadPreset(xmlFiles[i], preset))
        {
            presets[loadedCount] = preset;
            loadedCount++;
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

    for (int i = 0; i < MAX_PRESETS; ++i)
    {
        if (!presets[i].isEmpty)
        {
            juce::File presetFile = folder.getChildFile(presets[i].name + ".xml");
            presetManager.savePreset(presetFile, presets[i].name);
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
    presets[index] = preset;

    if (sessionFolder != juce::File() && !preset.isEmpty)
    {
        juce::File presetFile = sessionFolder.getChildFile(preset.name + ".xml");
        presetManager.savePreset(presetFile, preset.name);
    }

    notifySessionChanged();
}

void SessionManager::clearPreset(int index)
{
    jassert(index >= 0 && index < MAX_PRESETS);
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

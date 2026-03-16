#include "session_manager.h"
#include "factory_presets.h"

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

void SessionManager::setRootFolder(const juce::File& folder)
{
    rootFolder = folder;
}

juce::StringArray SessionManager::getCollectionNames() const
{
    juce::StringArray names;
    names.add ("Default"); // built-in factory collection, always available

    if (rootFolder.isDirectory())
    {
        juce::StringArray userNames;
        for (const auto& entry : juce::RangedDirectoryIterator(rootFolder, false, "*", juce::File::findDirectories))
            userNames.add(entry.getFile().getFileName());

        userNames.sort (true);
        names.addArray (userNames);
    }

    return names;
}

bool SessionManager::selectCollection(const juce::String& collectionName)
{
    if (collectionName == "Default")
    {
        loadFactorySession();
        return true;
    }

    if (!rootFolder.isDirectory())
        return false;

    juce::File collectionFolder = rootFolder.getChildFile(collectionName);
    if (!collectionFolder.isDirectory())
        return false;

    return loadSessionFromFolder(collectionFolder);
}

void SessionManager::loadFactorySession()
{
    for (auto& preset : presets)
        preset = Preset();

    sessionName = "Default";
    sessionFolder = juce::File(); // no folder — factory presets are read-only
    currentPresetIndex = -1;

    auto factoryPresets = FactoryPresets::build();
    for (int i = 0; i < MAX_PRESETS; ++i)
        presets[i] = factoryPresets[static_cast<size_t> (i)];

    notifySessionChanged();
}

bool SessionManager::createCollection(const juce::String& collectionName)
{
    if (!rootFolder.isDirectory())
        return false;

    juce::File collectionFolder = rootFolder.getChildFile(collectionName);
    if (collectionFolder.exists())
        return false;

    if (!collectionFolder.createDirectory())
        return false;

    return loadSessionFromFolder(collectionFolder);
}

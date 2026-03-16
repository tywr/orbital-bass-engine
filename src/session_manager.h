#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "preset_manager.h"
#include <array>

class SessionManager {
public:
    static constexpr int MAX_PRESETS = 5;

    SessionManager(PresetManager& pm);

    bool loadSessionFromFolder(const juce::File& folder);

    void saveSession(const juce::File& folder, const juce::String& sessionName);

    const std::array<Preset, MAX_PRESETS>& getPresets() const { return presets; }

    const Preset& getPreset(int index) const;

    void setPreset(int index, const Preset& preset);

    void clearPreset(int index);

    void clearSession();

    juce::String getSessionName() const { return sessionName; }

    juce::File getSessionFolder() const { return sessionFolder; }

    bool hasSessionFolder() const { return sessionFolder != juce::File(); }

    int getCurrentPresetIndex() const { return currentPresetIndex; }

    void setCurrentPresetIndex(int index);

    // Root folder & collection management
    void setRootFolder(const juce::File& folder);
    juce::File getRootFolder() const { return rootFolder; }
    bool hasRootFolder() const { return rootFolder != juce::File(); }

    juce::StringArray getCollectionNames() const;
    juce::String getCurrentCollectionName() const { return sessionName; }
    bool selectCollection(const juce::String& collectionName);
    bool createCollection(const juce::String& collectionName);

    // Loads the built-in factory session (no root folder required).
    // Called on first launch when no user session has been saved.
    void loadFactorySession();

    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void sessionChanged() = 0;
        virtual void currentPresetChanged(int newIndex) = 0;
    };

    void addListener(Listener* listener);
    void removeListener(Listener* listener);

private:
    PresetManager& presetManager;
    std::array<Preset, MAX_PRESETS> presets;
    juce::String sessionName;
    juce::File sessionFolder;
    juce::File rootFolder;
    int currentPresetIndex = -1;
    juce::ListenerList<Listener> listeners;

    void notifySessionChanged();
    void notifyCurrentPresetChanged();
};

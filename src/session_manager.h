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
    int currentPresetIndex = -1;
    juce::ListenerList<Listener> listeners;

    void notifySessionChanged();
    void notifyCurrentPresetChanged();
};

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../session_manager.h"
#include "colours.h"

class PresetSlot : public juce::Component
{
public:
    PresetSlot(int slotIndex);

    void setPreset(const Preset& preset);
    void setActive(bool shouldBeActive);
    bool isEmpty() const { return isEmptySlot; }

    std::function<void(int)> onClicked;

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

private:
    int index;
    juce::String presetName;
    bool isEmptySlot = true;
    bool isActive = false;
    bool isHovered = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetSlot)
};

class PresetBar : public juce::Component, public SessionManager::Listener
{
public:
    PresetBar(SessionManager& sm);
    ~PresetBar() override;

    void resized() override;
    void paint(juce::Graphics& g) override;

    void sessionChanged() override;
    void currentPresetChanged(int newIndex) override;

    std::function<void(int)> onPresetClicked;

private:
    SessionManager& sessionManager;

    std::array<std::unique_ptr<PresetSlot>, SessionManager::MAX_PRESETS> presetSlots;

    void updatePresetSlots();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetBar)
};

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "colours.h"

class IconButton : public juce::Component
{
public:
    enum IconType { Folder, Save };

    IconButton(IconType type);

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

    std::function<void()> onClick;

private:
    IconType iconType;
    bool isHovered = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IconButton)
};

class PresetIconButtons : public juce::Component
{
public:
    PresetIconButtons();
    ~PresetIconButtons() override;

    void resized() override;
    void paint(juce::Graphics& g) override;

    std::function<void()> onLoadSessionClicked;
    std::function<void()> onSavePresetClicked;

private:
    std::unique_ptr<IconButton> loadSessionButton;
    std::unique_ptr<IconButton> savePresetButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetIconButtons)
};

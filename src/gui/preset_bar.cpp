#include "preset_bar.h"
#include "colours.h"

PresetSlot::PresetSlot(int slotIndex) : index(slotIndex)
{
}

void PresetSlot::setPreset(const Preset& preset)
{
    presetName = preset.name;
    isEmptySlot = preset.isEmpty;
    repaint();
}

void PresetSlot::setActive(bool shouldBeActive)
{
    isActive = shouldBeActive;
    repaint();
}

void PresetSlot::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    if (isActive)
    {
        // Selected: orange square background
        g.setColour(ColourCodes::orange); // Orange
        g.fillRect(bounds);

        g.setColour(juce::Colours::black);
    }
    else
    {
        // Non-selected: black/transparent background with white text
        if (isHovered)
        {
            g.setColour(ColourCodes::white1);
        }
        else
        {
            g.setColour(isEmptySlot ? ColourCodes::grey2 : ColourCodes::white0);
        }
    }

    g.setFont(juce::Font("Typestar", 13.0f, juce::Font::plain));

    if (isEmptySlot)
    {
        g.drawText(
            juce::String(index + 1), bounds, juce::Justification::centred
        );
    }
    else
    {
        g.drawFittedText(
            presetName, bounds.toNearestInt(), juce::Justification::centred, 2
        );
    }
}

void PresetSlot::mouseDown(const juce::MouseEvent&)
{
    if (onClicked)
        onClicked(index);
}

void PresetSlot::mouseEnter(const juce::MouseEvent&)
{
    isHovered = true;
    repaint();
}

void PresetSlot::mouseExit(const juce::MouseEvent&)
{
    isHovered = false;
    repaint();
}

PresetBar::PresetBar(SessionManager& sm) : sessionManager(sm)
{
    for (int i = 0; i < SessionManager::MAX_PRESETS; ++i)
    {
        presetSlots[i] = std::make_unique<PresetSlot>(i);
        presetSlots[i]->onClicked = [this](int index)
        {
            if (onPresetClicked)
                onPresetClicked(index);
        };
        addAndMakeVisible(presetSlots[i].get());
    }

    sessionManager.addListener(this);

    updatePresetSlots();
}

PresetBar::~PresetBar()
{
    sessionManager.removeListener(this);
}

void PresetBar::resized()
{
    auto bounds = getLocalBounds();
    int slotWidth = bounds.getWidth() / SessionManager::MAX_PRESETS;

    for (int i = 0; i < SessionManager::MAX_PRESETS; ++i)
    {
        presetSlots[i]->setBounds(bounds.removeFromLeft(slotWidth));
    }
}

void PresetBar::paint(juce::Graphics& g)
{
    // Transparent background
    g.fillAll(juce::Colours::transparentBlack);
}

void PresetBar::sessionChanged()
{
    updatePresetSlots();
}

void PresetBar::currentPresetChanged(int newIndex)
{
    for (int i = 0; i < SessionManager::MAX_PRESETS; ++i)
    {
        presetSlots[i]->setActive(i == newIndex);
    }
}

void PresetBar::updatePresetSlots()
{
    for (int i = 0; i < SessionManager::MAX_PRESETS; ++i)
    {
        presetSlots[i]->setPreset(sessionManager.getPreset(i));
        presetSlots[i]->setActive(i == sessionManager.getCurrentPresetIndex());
    }
}

#include "preset_bar.h"

PresetSlot::PresetSlot(int slotIndex)
    : index(slotIndex)
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
        g.setColour(ColourCodes::blue2);
        g.fillRoundedRectangle(bounds, 4.0f);
    }
    else if (isHovered)
    {
        g.setColour(isEmptySlot ? ColourCodes::grey1 : ColourCodes::grey2);
        g.fillRoundedRectangle(bounds, 4.0f);
    }
    else
    {
        g.setColour(ColourCodes::bg2);
        g.fillRoundedRectangle(bounds, 4.0f);
    }

    g.setColour(ColourCodes::grey3);
    g.drawRoundedRectangle(bounds.reduced(1.0f), 4.0f, 1.0f);

    g.setColour(isEmptySlot ? ColourCodes::grey2 : ColourCodes::white0);
    g.setFont(12.0f);

    if (isEmptySlot)
    {
        g.drawText(juce::String(index + 1), bounds, juce::Justification::centred);
    }
    else
    {
        auto textBounds = bounds.reduced(4.0f);
        g.drawFittedText(presetName, textBounds.toNearestInt(), juce::Justification::centred, 2);
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

PresetBar::PresetBar(SessionManager& sm)
    : sessionManager(sm)
{
    for (int i = 0; i < SessionManager::MAX_PRESETS; ++i)
    {
        presetSlots[i] = std::make_unique<PresetSlot>(i);
        presetSlots[i]->onClicked = [this](int index) {
            if (onPresetClicked)
                onPresetClicked(index);
        };
        addAndMakeVisible(presetSlots[i].get());
    }

    loadSessionButton.setButtonText("Load Session");
    loadSessionButton.onClick = [this]() {
        if (onLoadSessionClicked)
            onLoadSessionClicked();
    };
    addAndMakeVisible(loadSessionButton);

    savePresetButton.setButtonText("Save Preset");
    savePresetButton.onClick = [this]() {
        if (onSavePresetClicked)
            onSavePresetClicked();
    };
    addAndMakeVisible(savePresetButton);

    sessionNameLabel.setJustificationType(juce::Justification::centredLeft);
    sessionNameLabel.setColour(juce::Label::textColourId, ColourCodes::white0);
    addAndMakeVisible(sessionNameLabel);

    sessionManager.addListener(this);

    updatePresetSlots();
    updateSessionLabel();
}

PresetBar::~PresetBar()
{
    sessionManager.removeListener(this);
}

void PresetBar::resized()
{
    auto bounds = getLocalBounds();
    bounds.reduce(4, 4);

    auto buttonWidth = 100;
    auto spacing = 4;

    loadSessionButton.setBounds(bounds.removeFromLeft(buttonWidth));
    bounds.removeFromLeft(spacing);

    savePresetButton.setBounds(bounds.removeFromLeft(buttonWidth));
    bounds.removeFromLeft(spacing);

    sessionNameLabel.setBounds(bounds.removeFromLeft(150));
    bounds.removeFromLeft(spacing);

    int slotWidth = (bounds.getWidth() - (SessionManager::MAX_PRESETS - 1) * spacing) / SessionManager::MAX_PRESETS;

    for (int i = 0; i < SessionManager::MAX_PRESETS; ++i)
    {
        presetSlots[i]->setBounds(bounds.removeFromLeft(slotWidth));
        if (i < SessionManager::MAX_PRESETS - 1)
            bounds.removeFromLeft(spacing);
    }
}

void PresetBar::paint(juce::Graphics& g)
{
    g.fillAll(ColourCodes::bg0);
}

void PresetBar::sessionChanged()
{
    updatePresetSlots();
    updateSessionLabel();
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

void PresetBar::updateSessionLabel()
{
    if (sessionManager.hasSessionFolder())
    {
        sessionNameLabel.setText("Session: " + sessionManager.getSessionName(), juce::dontSendNotification);
    }
    else
    {
        sessionNameLabel.setText("No session loaded", juce::dontSendNotification);
    }
}

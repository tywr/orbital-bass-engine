#include "preset_bar.h"

IconButton::IconButton(IconType type)
    : iconType(type)
{
}

void IconButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    if (isHovered)
    {
        g.setColour(ColourCodes::grey2);
        g.fillRoundedRectangle(bounds, 4.0f);
    }
    else
    {
        g.setColour(ColourCodes::bg2);
        g.fillRoundedRectangle(bounds, 4.0f);
    }

    g.setColour(ColourCodes::grey3);
    g.drawRoundedRectangle(bounds.reduced(1.0f), 4.0f, 1.0f);

    auto iconBounds = bounds.reduced(8.0f);
    g.setColour(ColourCodes::white0);

    if (iconType == Folder)
    {
        float folderHeight = iconBounds.getHeight() * 0.7f;
        float folderWidth = iconBounds.getWidth();
        float folderY = iconBounds.getCentreY() - folderHeight / 2.0f;
        float tabWidth = folderWidth * 0.4f;
        float tabHeight = folderHeight * 0.25f;

        juce::Path folderPath;
        folderPath.startNewSubPath(iconBounds.getX(), folderY + tabHeight);
        folderPath.lineTo(iconBounds.getX() + tabWidth, folderY + tabHeight);
        folderPath.lineTo(iconBounds.getX() + tabWidth + 3, folderY);
        folderPath.lineTo(iconBounds.getRight(), folderY);
        folderPath.lineTo(iconBounds.getRight(), folderY + folderHeight);
        folderPath.lineTo(iconBounds.getX(), folderY + folderHeight);
        folderPath.closeSubPath();

        g.strokePath(folderPath, juce::PathStrokeType(1.5f));
    }
    else if (iconType == Save)
    {
        float arrowWidth = iconBounds.getWidth() * 0.8f;
        float arrowHeight = iconBounds.getHeight() * 0.8f;
        float centerX = iconBounds.getCentreX();
        float centerY = iconBounds.getCentreY();

        juce::Path arrowPath;
        arrowPath.startNewSubPath(centerX, centerY - arrowHeight / 2.0f);
        arrowPath.lineTo(centerX, centerY + arrowHeight / 2.0f);

        arrowPath.startNewSubPath(centerX - arrowWidth / 3.0f, centerY + arrowHeight / 6.0f);
        arrowPath.lineTo(centerX, centerY + arrowHeight / 2.0f);
        arrowPath.lineTo(centerX + arrowWidth / 3.0f, centerY + arrowHeight / 6.0f);

        float baseY = centerY + arrowHeight / 2.0f + 2.0f;
        arrowPath.startNewSubPath(centerX - arrowWidth / 2.0f, baseY);
        arrowPath.lineTo(centerX + arrowWidth / 2.0f, baseY);

        g.strokePath(arrowPath, juce::PathStrokeType(1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
}

void IconButton::mouseDown(const juce::MouseEvent&)
{
    if (onClick)
        onClick();
}

void IconButton::mouseEnter(const juce::MouseEvent&)
{
    isHovered = true;
    repaint();
}

void IconButton::mouseExit(const juce::MouseEvent&)
{
    isHovered = false;
    repaint();
}

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

    loadSessionButton = std::make_unique<IconButton>(IconButton::Folder);
    loadSessionButton->onClick = [this]() {
        if (onLoadSessionClicked)
            onLoadSessionClicked();
    };
    addAndMakeVisible(loadSessionButton.get());

    savePresetButton = std::make_unique<IconButton>(IconButton::Save);
    savePresetButton->onClick = [this]() {
        if (onSavePresetClicked)
            onSavePresetClicked();
    };
    addAndMakeVisible(savePresetButton.get());

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

    auto iconButtonSize = bounds.getHeight();
    auto spacing = 4;

    loadSessionButton->setBounds(bounds.removeFromLeft(iconButtonSize).reduced(2));
    bounds.removeFromLeft(spacing);

    savePresetButton->setBounds(bounds.removeFromLeft(iconButtonSize).reduced(2));
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

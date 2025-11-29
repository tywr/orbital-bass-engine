#include "preset_bar.h"

IconButton::IconButton(IconType type)
    : iconType(type)
{
}

void IconButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    juce::Colour iconColour;
    if (isHovered)
    {
        iconColour = ColourCodes::white1;
    }
    else
    {
        iconColour = ColourCodes::white0;
    }

    auto iconBounds = bounds.reduced(10.0f);
    g.setColour(iconColour);

    if (iconType == Folder)
    {
        float folderHeight = iconBounds.getHeight() * 0.55f;
        float folderWidth = iconBounds.getWidth() * 0.7f;
        float folderX = iconBounds.getCentreX() - folderWidth / 2.0f;
        float folderY = iconBounds.getCentreY() - folderHeight / 2.0f;
        float tabWidth = folderWidth * 0.4f;
        float tabHeight = folderHeight * 0.25f;

        juce::Path folderPath;
        folderPath.startNewSubPath(folderX, folderY + tabHeight);
        folderPath.lineTo(folderX + tabWidth, folderY + tabHeight);
        folderPath.lineTo(folderX + tabWidth + 2, folderY);
        folderPath.lineTo(folderX + folderWidth, folderY);
        folderPath.lineTo(folderX + folderWidth, folderY + folderHeight);
        folderPath.lineTo(folderX, folderY + folderHeight);
        folderPath.closeSubPath();

        g.strokePath(folderPath, juce::PathStrokeType(1.2f));
    }
    else if (iconType == Save)
    {
        float arrowWidth = iconBounds.getWidth() * 0.5f;
        float arrowHeight = iconBounds.getHeight() * 0.6f;
        float centerX = iconBounds.getCentreX();
        float centerY = iconBounds.getCentreY();

        juce::Path arrowPath;
        arrowPath.startNewSubPath(centerX, centerY - arrowHeight / 2.0f);
        arrowPath.lineTo(centerX, centerY + arrowHeight / 2.0f);

        arrowPath.startNewSubPath(centerX - arrowWidth / 3.0f, centerY + arrowHeight / 6.0f);
        arrowPath.lineTo(centerX, centerY + arrowHeight / 2.0f);
        arrowPath.lineTo(centerX + arrowWidth / 3.0f, centerY + arrowHeight / 6.0f);

        float baseY = centerY + arrowHeight / 2.0f + 1.5f;
        arrowPath.startNewSubPath(centerX - arrowWidth / 2.0f, baseY);
        arrowPath.lineTo(centerX + arrowWidth / 2.0f, baseY);

        g.strokePath(arrowPath, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
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

    juce::Colour textColour;

    if (isActive)
    {
        textColour = ColourCodes::blue2;
    }
    else if (isHovered)
    {
        textColour = isEmptySlot ? ColourCodes::grey3 : ColourCodes::white1;
    }
    else
    {
        textColour = isEmptySlot ? ColourCodes::grey2 : ColourCodes::white0;
    }

    g.setColour(textColour);
    g.setFont(juce::Font("Oxanium", 11.0f, juce::Font::plain));

    if (isEmptySlot)
    {
        g.drawText(juce::String(index + 1), bounds, juce::Justification::centred);
    }
    else
    {
        g.drawFittedText(presetName, bounds.toNearestInt(), juce::Justification::centred, 2);
    }

    if (isActive)
    {
        float underlineY = bounds.getBottom() - 2.0f;
        g.drawLine(bounds.getX() + 4.0f, underlineY, bounds.getRight() - 4.0f, underlineY, 2.0f);
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
    auto iconButtonSize = bounds.getHeight() - 4;
    auto spacing = 6;

    bounds.removeFromLeft(2);

    loadSessionButton->setBounds(bounds.removeFromLeft(iconButtonSize));
    bounds.removeFromLeft(spacing);

    savePresetButton->setBounds(bounds.removeFromLeft(iconButtonSize));
    bounds.removeFromLeft(spacing * 2);

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

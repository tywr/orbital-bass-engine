#include "preset_icon_buttons.h"

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
    else if (iconType == Reload)
    {
        float radius = iconBounds.getWidth() * 0.35f;
        float centerX = iconBounds.getCentreX();
        float centerY = iconBounds.getCentreY();

        juce::Path reloadPath;

        // Arrowhead angle and position
        float arrowheadAngle = juce::MathConstants<float>::pi * 0.25f; // 45 degrees
        float arrowSize = radius * 0.4f;
        float arrowWidth = 0.35f;

        // Calculate arrowhead tip position
        float tipX = centerX + radius * std::cos(arrowheadAngle);
        float tipY = centerY + radius * std::sin(arrowheadAngle);

        // Draw circular arc that ends at the arrowhead
        // Go from ~117° counter-clockwise to 45° (the long way = 288° arc)
        // To go counter-clockwise from 117° to 45°, we go to 45° + 360° = 405°
        reloadPath.addCentredArc(
            centerX, centerY,
            radius, radius,
            0.0f,
            juce::MathConstants<float>::pi * 0.65f, // Start at ~117°
            arrowheadAngle + juce::MathConstants<float>::pi * 2.0f, // End at 45° + 360° = 405°
            true // Start as new subpath
        );

        // For counter-clockwise motion ending at arrowheadAngle
        // Tangent is perpendicular: angle + π/2
        float tangentAngle = arrowheadAngle + juce::MathConstants<float>::pi * 0.5f;

        // Draw arrowhead pointing forward (in direction of motion)
        reloadPath.startNewSubPath(tipX, tipY);
        reloadPath.lineTo(
            tipX + arrowSize * std::cos(tangentAngle - arrowWidth),
            tipY + arrowSize * std::sin(tangentAngle - arrowWidth)
        );

        reloadPath.startNewSubPath(tipX, tipY);
        reloadPath.lineTo(
            tipX + arrowSize * std::cos(tangentAngle + arrowWidth),
            tipY + arrowSize * std::sin(tangentAngle + arrowWidth)
        );

        g.strokePath(reloadPath, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
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

PresetIconButtons::PresetIconButtons()
{
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

    reloadPresetButton = std::make_unique<IconButton>(IconButton::Reload);
    reloadPresetButton->onClick = [this]() {
        if (onReloadPresetClicked)
            onReloadPresetClicked();
    };
    addAndMakeVisible(reloadPresetButton.get());
}

PresetIconButtons::~PresetIconButtons()
{
}

void PresetIconButtons::resized()
{
    auto bounds = getLocalBounds();
    auto iconButtonSize = bounds.getHeight() - 4;
    auto spacing = 6;

    bounds.removeFromLeft(2);

    loadSessionButton->setBounds(bounds.removeFromLeft(iconButtonSize));
    bounds.removeFromLeft(spacing);

    savePresetButton->setBounds(bounds.removeFromLeft(iconButtonSize));
    bounds.removeFromLeft(spacing);

    reloadPresetButton->setBounds(bounds.removeFromLeft(iconButtonSize));
}

void PresetIconButtons::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::transparentBlack);
}

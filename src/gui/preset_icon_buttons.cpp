#include "preset_icon_buttons.h"

IconButton::IconButton(IconType type)
    : iconType(type)
{
}

void IconButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    juce::Colour iconColour = isHovered ? ColourCodes::white1 : ColourCodes::white0;
    g.setColour(iconColour);

    float iconSize = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto iconBounds = bounds.withSizeKeepingCentre(iconSize, iconSize);
    float centerX = iconBounds.getCentreX();
    float centerY = iconBounds.getCentreY();
    float stroke = 1.4f;
    juce::PathStrokeType strokeType(stroke, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

    if (iconType == Folder)
    {
        float w = iconSize;
        float h = iconSize * 0.75f;
        float x = centerX - w / 2.0f;
        float y = centerY - h / 2.0f;
        float tabW = w * 0.4f;
        float tabH = h * 0.22f;

        juce::Path path;
        path.startNewSubPath(x, y + tabH);
        path.lineTo(x + tabW, y + tabH);
        path.lineTo(x + tabW + tabH * 0.6f, y);
        path.lineTo(x + w, y);
        path.lineTo(x + w, y + h);
        path.lineTo(x, y + h);
        path.closeSubPath();

        g.strokePath(path, strokeType);
    }
    else if (iconType == NewCollection)
    {
        float arm = iconSize * 0.45f;

        juce::Path path;
        path.startNewSubPath(centerX - arm, centerY);
        path.lineTo(centerX + arm, centerY);
        path.startNewSubPath(centerX, centerY - arm);
        path.lineTo(centerX, centerY + arm);

        g.strokePath(path, strokeType);
    }
    else if (iconType == Save)
    {
        float halfW = iconSize * 0.45f;
        float halfH = iconSize * 0.45f;
        float chevronH = halfH * 0.45f;

        juce::Path path;
        // Vertical shaft
        path.startNewSubPath(centerX, centerY - halfH);
        path.lineTo(centerX, centerY + halfH);
        // Chevron
        path.startNewSubPath(centerX - halfW * 0.6f, centerY + halfH - chevronH);
        path.lineTo(centerX, centerY + halfH);
        path.lineTo(centerX + halfW * 0.6f, centerY + halfH - chevronH);
        // Base line
        float baseY = centerY + halfH + stroke * 1.5f;
        path.startNewSubPath(centerX - halfW, baseY);
        path.lineTo(centerX + halfW, baseY);

        g.strokePath(path, strokeType);
    }
    else if (iconType == Reload)
    {
        float radius = iconSize * 0.42f;

        juce::Path path;

        float arrowheadAngle = juce::MathConstants<float>::pi * 0.25f;
        float arrowLen = radius * 0.55f;
        float arrowSpread = 0.7f;

        float tipX = centerX + radius * std::cos(arrowheadAngle);
        float tipY = centerY + radius * std::sin(arrowheadAngle);

        path.addCentredArc(
            centerX, centerY,
            radius, radius,
            0.0f,
            juce::MathConstants<float>::pi * 0.75f,
            arrowheadAngle + juce::MathConstants<float>::pi * 2.0f,
            true
        );

        float tangent = arrowheadAngle + juce::MathConstants<float>::pi * 0.58f;

        path.startNewSubPath(tipX, tipY);
        path.lineTo(
            tipX + arrowLen * std::cos(tangent - arrowSpread),
            tipY + arrowLen * std::sin(tangent - arrowSpread)
        );
        path.startNewSubPath(tipX, tipY);
        path.lineTo(
            tipX + arrowLen * std::cos(tangent + arrowSpread),
            tipY + arrowLen * std::sin(tangent + arrowSpread)
        );

        g.strokePath(path, strokeType);
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

    newCollectionButton = std::make_unique<IconButton>(IconButton::NewCollection);
    newCollectionButton->onClick = [this]() {
        if (onNewCollectionClicked)
            onNewCollectionClicked();
    };
    addAndMakeVisible(newCollectionButton.get());

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

    newCollectionButton->setBounds(bounds.removeFromLeft(iconButtonSize));
    bounds.removeFromLeft(spacing);

    savePresetButton->setBounds(bounds.removeFromLeft(iconButtonSize));
    bounds.removeFromLeft(spacing);

    reloadPresetButton->setBounds(bounds.removeFromLeft(iconButtonSize));
}

void PresetIconButtons::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::transparentBlack);
}

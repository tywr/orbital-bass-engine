#include "tuner.h"
#include <cmath>
#include <juce_gui_basics/juce_gui_basics.h>

CloseButton::CloseButton() {}

void CloseButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float iconSize = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto iconBounds = bounds.withSizeKeepingCentre(iconSize, iconSize);
    float cx = iconBounds.getCentreX();
    float cy = iconBounds.getCentreY();
    float arm = iconSize * 0.4f;

    juce::Colour colour = isHovered ? ColourCodes::orange : ColourCodes::white0;
    g.setColour(colour);

    juce::Path path;
    path.startNewSubPath(cx - arm, cy - arm);
    path.lineTo(cx + arm, cy + arm);
    path.startNewSubPath(cx + arm, cy - arm);
    path.lineTo(cx - arm, cy + arm);

    g.strokePath(path, juce::PathStrokeType(1.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void CloseButton::mouseDown(const juce::MouseEvent&)
{
    if (onClick)
        onClick();
}

void CloseButton::mouseEnter(const juce::MouseEvent&)
{
    isHovered = true;
    repaint();
}

void CloseButton::mouseExit(const juce::MouseEvent&)
{
    isHovered = false;
    repaint();
}

Tuner::Tuner(juce::Value& v) : pitch_value(v)
{
    startTimerHz(60);
    smoothedFreq.reset(60, 0.1);

    pitch_value.addListener(this);
    pitch_value.setValue(0.0f);

    addAndMakeVisible(closeButton);
    closeButton.onClick = [this]()
    {
        if (onClose)
            onClose();
    };
}

Tuner::~Tuner()
{
    pitch_value.removeListener(this);
}

void Tuner::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    bool inTune = currentFreq > 0.0f && std::abs(centsDeviation) < inTuneThreshold;
    juce::Colour accentColour = inTune ? ColourCodes::orange : ColourCodes::grey3;

    auto bounds = getLocalBounds().reduced(100).toFloat();
    g.setColour(accentColour.withAlpha(0.4f));
    g.drawRect(bounds, 2.0f);

    auto innerBounds = bounds.reduced(40.0f);

    // Note label
    g.setColour(inTune ? ColourCodes::orange : ColourCodes::white0);
    g.setFont(48.0f);
    g.drawText(
        noteLabel, innerBounds.removeFromTop(60.0f),
        juce::Justification::centred
    );

    // Pitch bar
    auto sliderArea = innerBounds.reduced(20.0f, 40.0f);
    float sliderHeight = 12.0f;
    auto sliderBounds =
        sliderArea.withHeight(sliderHeight).withCentre(innerBounds.getCentre());

    // Bar background
    g.setColour(ColourCodes::bg2);
    g.fillRect(sliderBounds);

    // Tick marks
    float centerX = sliderBounds.getCentreX();
    float tickTop = sliderBounds.getY() - 4.0f;
    float tickBottom = sliderBounds.getBottom() + 4.0f;
    g.setColour(ColourCodes::grey2);
    for (int i = -4; i <= 4; ++i)
    {
        float tickX = centerX + i * (sliderBounds.getWidth() / 10.0f);
        float h = (i == 0) ? 0.0f : 2.0f;
        g.drawLine(tickX, tickTop + h, tickX, tickBottom - h, 1.0f);
    }

    // Center marker
    g.setColour(accentColour);
    g.drawLine(centerX, tickTop, centerX, tickBottom, 2.0f);

    // Pitch indicator
    if (currentFreq > 0.0f)
    {
        float normalizedPos = juce::jlimit(-1.0f, 1.0f, centsDeviation / 50.0f);
        float indicatorX =
            centerX + normalizedPos * (sliderBounds.getWidth() / 2.0f - 8.0f);
        float indicatorW = 6.0f;
        float indicatorH = sliderHeight + 10.0f;

        g.setColour(inTune ? ColourCodes::orange : ColourCodes::white0);
        g.fillRect(
            indicatorX - indicatorW / 2.0f,
            sliderBounds.getCentreY() - indicatorH / 2.0f,
            indicatorW, indicatorH
        );
    }
}

void Tuner::resized()
{
    auto frameBounds = getLocalBounds().reduced(100);
    int const buttonSize = 32;
    int const margin = 10;

    closeButton.setBounds(
        frameBounds.getRight() - buttonSize - margin,
        frameBounds.getY() + margin,
        buttonSize, buttonSize
    );
}

void Tuner::valueChanged(juce::Value& newValue)
{
    float freq = static_cast<float>(newValue.getValue());
    targetFreq.store(freq);
}

void Tuner::timerCallback()
{
    updatePitchDisplay();
    repaint();
}

void Tuner::visibilityChanged()
{
    juce::MessageManager::callAsync(
        [this]
        {
            if (isShowing())
                startTimerHz(60);
            else
                stopTimer();
        }
    );
}

void Tuner::updatePitchDisplay()
{
    float freq = targetFreq.load();

    if (freq <= 0.0f)
    {
        currentFreq = 0.0f;
        smoothedFreq.setCurrentAndTargetValue(0.0f);
        noteLabel = "-";
        centsDeviation = 0.0f;
        return;
    }

    smoothedFreq.setTargetValue(freq);
    currentFreq = smoothedFreq.getNextValue();

    // Calculate note from frequency (A4 = 440Hz)
    float semitonesFromA4 = 12.0f * std::log2(currentFreq / 440.0f);
    int nearestNote = static_cast<int>(std::round(semitonesFromA4));
    centsDeviation = (semitonesFromA4 - nearestNote) * 100.0f;

    // Convert to note index (A = 0)
    int noteIndex = ((nearestNote % 12) + 12) % 12;
    noteLabel = getNoteLabel(noteIndex);
}

juce::String Tuner::getNoteLabel(int noteIndex) const
{
    static const char* notes[] = {"A",  "A#", "B", "C",  "C#", "D",
                                  "D#", "E",  "F", "F#", "G",  "G#"};
    return notes[noteIndex];
}

#include "tuner.h"
#include <cmath>
#include <juce_gui_basics/juce_gui_basics.h>

Tuner::Tuner(juce::Value& v) : pitch_value(v)
{
    startTimerHz(60);
    smoothedFreq.reset(60, 0.1);

    pitch_value.addListener(this);
    pitch_value.setValue(0.0f);

    addAndMakeVisible(closeButton);
    closeButton.setButtonText("X");
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
    auto bounds = getLocalBounds().reduced(100).toFloat();
    g.setColour(ColourCodes::white0.withAlpha(0.3f));
    g.drawRoundedRectangle(bounds, 20.0f, 5.0f);

    auto innerBounds = bounds.reduced(40.0f);

    // Draw note label
    g.setColour(ColourCodes::white0);
    g.setFont(48.0f);
    g.drawText(
        noteLabel, innerBounds.removeFromTop(60.0f),
        juce::Justification::centred
    );

    // Draw pitch slider
    auto sliderArea = innerBounds.reduced(20.0f, 40.0f);
    float sliderHeight = 20.0f;
    auto sliderBounds =
        sliderArea.withHeight(sliderHeight).withCentre(innerBounds.getCentre());

    // Slider background
    g.setColour(ColourCodes::white0.withAlpha(0.2f));
    g.fillRoundedRectangle(sliderBounds, 10.0f);

    // Center marker
    float centerX = sliderBounds.getCentreX();
    g.setColour(ColourCodes::white0.withAlpha(0.5f));
    g.drawLine(
        centerX, sliderBounds.getY() - 5.0f, centerX,
        sliderBounds.getBottom() + 5.0f, 2.0f
    );

    // Pitch indicator
    if (currentFreq > 0.0f)
    {
        bool inTune = std::abs(centsDeviation) < inTuneThreshold;
        g.setColour(inTune ? juce::Colours::green : ColourCodes::white0);

        float normalizedPos = juce::jlimit(-1.0f, 1.0f, centsDeviation / 50.0f);
        float indicatorX =
            centerX + normalizedPos * (sliderBounds.getWidth() / 2.0f - 10.0f);
        float indicatorSize = 16.0f;

        g.fillEllipse(
            indicatorX - indicatorSize / 2.0f,
            sliderBounds.getCentreY() - indicatorSize / 2.0f, indicatorSize,
            indicatorSize
        );
    }
}

void Tuner::resized()
{
    auto bounds = getLocalBounds();
    int const buttonSize = 40;
    int const padding = 20;

    closeButton.setBounds(
        bounds.getWidth() - buttonSize - padding, padding, buttonSize,
        buttonSize
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

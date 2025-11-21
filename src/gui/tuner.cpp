#include "tuner.h"
#include <juce_gui_basics/juce_gui_basics.h>

Tuner::Tuner()
{
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
}

void Tuner::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    auto centerArea = getLocalBounds().reduced(100);
    g.setColour(ColourCodes::white0.withAlpha(0.3f));
    g.drawRect(centerArea, 2);
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

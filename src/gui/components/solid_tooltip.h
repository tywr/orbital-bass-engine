#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class SolidTooltip : public juce::Label
{
  public:
    void paint(juce::Graphics& g) override
    {
        g.fillAll(
            findColour(juce::Label::backgroundColourId)
        );
        juce::Label::paint(g);
    }
};

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class SolidTooltip : public juce::Label
{
  public:
    void paint(juce::Graphics& g) override
    {
        g.fillAll(findColour(juce::Label::backgroundColourId));
        juce::Label::paint(g);
    }

    void setBounds(juce::Rectangle<int> newBounds)
    {
        if (auto* parent = getParentComponent())
        {
            auto parentBounds = parent->getLocalBounds();
            newBounds = newBounds.getIntersection(parentBounds);
        }
        juce::Label::setBounds(newBounds);
    }

    void setBounds(int x, int y, int w, int h)
    {
        setBounds(juce::Rectangle<int>(x, y, w, h));
    }
};

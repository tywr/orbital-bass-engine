#pragma once
#include "../../assets/impulse_response_binary.h"
#include "../colours.h"
#include "designs/modern_410.h"
#include <juce_gui_basics/juce_gui_basics.h>

struct IRType
{
    juce::ToggleButton* button;
    juce::String id;
    const char* data;
    int size;
};

class Modern410ToggleButton : public juce::ToggleButton
{
  public:
    Modern410ToggleButton(IRType t)
    {
    }
    ~Modern410ToggleButton() override
    {
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        juce::Colour c;
        if (getToggleState())
        {
            c = findColour(juce::ToggleButton::textColourId);
        }
        else
        {
            c = findColour(juce::ToggleButton::textColourId).withAlpha(0.3f);
        }
        paintIconModern410(g, bounds, c);
    }
};

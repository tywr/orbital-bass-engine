#pragma once
#include "../colours.h"
#include "designs/borealis.h"
#include "designs/helios.h"
#include <juce_gui_basics/juce_gui_basics.h>

struct AmpType
{
    // juce::ToggleButton* button;
    juce::String id;
    juce::Colour colour1;
    juce::Colour colour2;
};

class HeliosToggleButton : public juce::ToggleButton
{
  public:
    HeliosToggleButton(AmpType t)
    {
        colour1 = t.colour1;
        colour2 = t.colour2;
    }
    ~HeliosToggleButton() override
    {
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        juce::Colour c1;
        juce::Colour c2;
        if (getToggleState())
        {
            c1 = colour1;
            c2 = colour2;
        }
        else
        {
            c1 = findColour(juce::ToggleButton::textColourId);
            c2 = findColour(juce::ToggleButton::textColourId);
        }
        paintSunFigureHelios(g, bounds, c1, c2);
    }

  private:
    juce::Colour colour1;
    juce::Colour colour2;
};

class BorealisToggleButton : public juce::ToggleButton
{
  public:
    BorealisToggleButton(AmpType t)
    {
        colour1 = t.colour1;
        colour2 = t.colour2;
    }
    ~BorealisToggleButton() override
    {
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        juce::Colour c1;
        juce::Colour c2;
        if (getToggleState())
        {
            c1 = colour1;
            c2 = colour2;
        }
        else
        {
            c1 = findColour(juce::ToggleButton::textColourId);
            c2 = findColour(juce::ToggleButton::textColourId);
        }
        paintIconBorealis(g, bounds, c1, c2);
    }

  private:
    juce::Colour colour1;
    juce::Colour colour2;
};

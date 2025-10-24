#pragma once
#include "../../assets/impulse_response_binary.h"
#include "../colours.h"
#include "designs/classic_810.h"
#include "designs/crunchy_212.h"
#include "designs/modern_410.h"
#include "designs/vintage_b15.h"
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

class Classic810ToggleButton : public juce::ToggleButton
{
  public:
    Classic810ToggleButton(IRType t)
    {
    }
    ~Classic810ToggleButton() override
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
        paintIconClassic810(g, bounds, c);
    }
};

class Crunchy212ToggleButton : public juce::ToggleButton
{
  public:
    Crunchy212ToggleButton(IRType t)
    {
    }
    ~Crunchy212ToggleButton() override
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
        paintIconCrunchy212(g, bounds, c);
    }
};

class VintageB15ToggleButton : public juce::ToggleButton
{
  public:
    VintageB15ToggleButton(IRType t)
    {
    }
    ~VintageB15ToggleButton() override
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
        paintIconVintageB15(g, bounds, c);
    }
};

#pragma once
#include "designs/borealis.h"
#include "designs/helios.h"
#include "designs/nebula.h"

struct AmpType
{
    juce::ToggleButton* button;
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
    };
    ~HeliosToggleButton() {};

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
    };

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
    };
    ~BorealisToggleButton() {};

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
    };

  private:
    juce::Colour colour1;
    juce::Colour colour2;
};

class NebulaToggleButton : public juce::ToggleButton
{
  public:
    NebulaToggleButton(AmpType t)
    {
        colour1 = t.colour1;
        colour2 = t.colour2;
    };
    ~NebulaToggleButton() {};

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
        paintIconNebula(g, bounds, c1, c2);
    };

  private:
    juce::Colour colour1;
    juce::Colour colour2;
};

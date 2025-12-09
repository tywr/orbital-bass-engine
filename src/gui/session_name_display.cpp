#include "session_name_display.h"

SessionNameDisplay::SessionNameDisplay(SessionManager& sm)
    : sessionManager(sm)
{
    sessionManager.addListener(this);
    updateSessionName();
}

SessionNameDisplay::~SessionNameDisplay()
{
    sessionManager.removeListener(this);
}

void SessionNameDisplay::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::transparentBlack);

    g.setColour(ColourCodes::white0);
    g.setFont(juce::Font(juce::FontOptions("Typestar", 13.0f, juce::Font::plain)));

    auto bounds = getLocalBounds();
    if (sessionName.isEmpty())
    {
        g.setColour(ColourCodes::grey2);
        g.drawText("No Session", bounds, juce::Justification::centred);
    }
    else
    {
        g.drawText(sessionName, bounds, juce::Justification::centred);
    }
}

void SessionNameDisplay::resized()
{
}

void SessionNameDisplay::sessionChanged()
{
    updateSessionName();
    repaint();
}

void SessionNameDisplay::currentPresetChanged(int newIndex)
{
    juce::ignoreUnused(newIndex);
}

void SessionNameDisplay::updateSessionName()
{
    if (sessionManager.hasSessionFolder())
    {
        auto sessionFolder = sessionManager.getSessionFolder();
        sessionName = sessionFolder.getFileName();
    }
    else
    {
        sessionName = "";
    }
}

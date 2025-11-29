#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../session_manager.h"
#include "colours.h"

class SessionNameDisplay : public juce::Component, public SessionManager::Listener
{
public:
    SessionNameDisplay(SessionManager& sm);
    ~SessionNameDisplay() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void sessionChanged() override;
    void currentPresetChanged(int newIndex) override;

private:
    SessionManager& sessionManager;
    juce::String sessionName;

    void updateSessionName();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SessionNameDisplay)
};

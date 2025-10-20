#include "plugin_audio_processor.h"
#include "plugin_editor.h"

class MonoStandaloneWindow : public juce::StandaloneFilterWindow
{
public:
    MonoStandaloneWindow(const juce::String& title,
                         juce::Colour backgroundColour)
        : juce::StandaloneFilterWindow(title, backgroundColour)
    {
        // Force mono input and output
        createAudioDeviceManager(1, 1);
        AudioDeviceManager::AudioDeviceSetup setup;
        deviceManager.getAudioDeviceSetup(setup);
        setup.inputChannels.clear();
        setup.inputChannels.set(1, true);  // channel 2 = index 1

        // Apply the new setup
        deviceManager.setAudioDeviceSetup(setup, true);
    }
};

class MonoStandaloneApp : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override       { return "MyPlugin"; }
    const juce::String getApplicationVersion() override    { return "1.0"; }

    void initialise(const juce::String&) override
    {
        mainWindow.reset(new MonoStandaloneWindow(getApplicationName(),
                                                  juce::Colours::darkgrey));
    }

    void shutdown() override { mainWindow = nullptr; }

private:
    std::unique_ptr<MonoStandaloneWindow> mainWindow;
};

START_JUCE_APPLICATION (MonoStandaloneApp)


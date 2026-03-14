#include "plugin_editor.h"

#include "gui/looks/base_look_and_feel.h"
#include "plugin_audio_processor.h"
#include <juce_core/juce_core.h>

using namespace juce;

//==============================================================================
PluginEditor::PluginEditor(
    PluginAudioProcessor& p, juce::AudioProcessorValueTreeState& params
)
    : AudioProcessorEditor(&p), processorRef(p), parameters(params),
      header(params, processorRef.inputLevel, processorRef.outputLevel, processorRef.getSessionManager()),
      panels(params, processorRef.compressorGainReductionDb),
      tuner(processorRef.currentPitch)
{

    setLookAndFeel(new BaseLookAndFeel());
    setSize(1080, 720);
    addAndMakeVisible(header);
    addAndMakeVisible(panels);

    auto& presetIconButtons = header.getPresetIconButtons();
    auto& presetBar = header.getPresetBar();
    auto& sessionNameDisplay = header.getSessionNameDisplay();

    presetIconButtons.onLoadSessionClicked = [this]() { handleLoadSession(); };
    presetIconButtons.onSavePresetClicked = [this]() { handleSavePreset(); };
    presetIconButtons.onReloadPresetClicked = [this]() { handleReloadPreset(); };
    presetIconButtons.onNewCollectionClicked = [this]() { handleNewCollection(); };
    presetBar.onPresetClicked = [this](int index) { handlePresetClicked(index); };
    sessionNameDisplay.onCollectionSelected = [this](const juce::String& name) { handleCollectionSelected(name); };

    addChildComponent(tuner);
    tuner.onClose = [this]() { hideTuner(); };
    header.onTunerClicked = [this]() { showTuner(); };

    juce::ignoreUnused(processorRef);

    getLookAndFeel().setColour(
        juce::Slider::thumbColourId, juce::Colours::lightgreen
    );
}

PluginEditor::~PluginEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void PluginEditor::paint(juce::Graphics& g)
{
    float scale = g.getInternalContext().getPhysicalPixelScaleFactor();
    if (!is_background_drawn)
    {
        paintBackground(scale);
        is_background_drawn = true;
    }
    auto bounds = getLocalBounds();
    g.drawImageAt(background, bounds.getX(), bounds.getY());
}

void PluginEditor::paintBackground(float scale)
{
    int width = static_cast<int>(scale * getWidth());
    int height = static_cast<int>(scale * getHeight());
    background = juce::Image(juce::Image::ARGB, width, height, true);

    juce::Graphics g(background);
    juce::Graphics cache(background);
    g.fillAll(juce::Colours::black);

    juce::Random random(3);
    const int gridSize = 30; // Space between potential dots

    for (int x = 0; x < getWidth(); x += gridSize)
    {
        for (int y = 0; y < getHeight(); y += gridSize)
        {
            if (random.nextFloat() > 0.7f) // 30% chance of dot
            {
                float offsetX = random.nextFloat() * gridSize;
                float offsetY = random.nextFloat() * gridSize;
                float size = random.nextFloat() * 3.0f + 0.5f;
                float alpha = random.nextFloat() * 0.5f + 0.05f;

                g.setColour(juce::Colours::white.withAlpha(alpha));
                g.fillEllipse(x + offsetX, y + offsetY, size, size);
            }
        }
    }
}

void PluginEditor::resized()
{
    auto bounds = getLocalBounds();

    const float header_ratio = 0.1f;
    const int header_height = static_cast<int>(getHeight() * header_ratio);
    header.setBounds(bounds.removeFromTop(header_height));

    panels.setBounds(bounds);
    tuner.setBounds(getLocalBounds());
}

void PluginEditor::showTuner()
{
    tuner.setVisible(true);
    tuner.toFront(true);
    processorRef.setTunerBypass(false);
}

void PluginEditor::hideTuner()
{
    tuner.setVisible(false);
    processorRef.setTunerBypass(true);
}

void PluginEditor::handlePresetClicked(int index)
{
    auto& sessionManager = processorRef.getSessionManager();
    const auto& preset = sessionManager.getPreset(index);

    sessionManager.setCurrentPresetIndex(index);
    processorRef.saveCurrentPresetIndex(index);

    if (!preset.isEmpty)
    {
        processorRef.getPresetManager().applyPreset(preset);
    }
}

void PluginEditor::handleLoadSession()
{
    auto& sessionManager = processorRef.getSessionManager();
    juce::File startDir = sessionManager.hasRootFolder()
        ? sessionManager.getRootFolder()
        : juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    auto chooser = std::make_shared<juce::FileChooser>(
        "Select your preset collections root folder",
        startDir
    );

    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories;

    chooser->launchAsync(chooserFlags, [this, chooser](const juce::FileChooser& fc)
    {
        auto selectedFolder = fc.getResult();
        if (selectedFolder == juce::File())
            return;

        if (processorRef.setRootFolder(selectedFolder))
        {
            auto collections = processorRef.getSessionManager().getCollectionNames();

            juce::String message = "Root folder set to: " + selectedFolder.getFullPathName() +
                "\n\nFound " + juce::String(collections.size()) + " collection" +
                (collections.size() == 1 ? "" : "s") + ".";

            if (collections.size() == 0)
                message += "\n\nUse the + button to create a new preset collection.";

            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::InfoIcon,
                "Root Folder Set",
                message
            );
        }
        else
        {
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon,
                "Invalid Folder",
                "Could not set root folder: " + selectedFolder.getFullPathName()
            );
        }
    });
}

void PluginEditor::handleSavePreset()
{
    auto& sessionManager = processorRef.getSessionManager();
    int currentIndex = sessionManager.getCurrentPresetIndex();

    if (currentIndex < 0 || currentIndex >= SessionManager::MAX_PRESETS)
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "No Preset Selected",
            "Please select a preset slot first by clicking on one."
        );
        return;
    }

    const auto& currentPreset = sessionManager.getPreset(currentIndex);

    juce::String defaultName = currentPreset.isEmpty ?
        "Preset " + juce::String(currentIndex + 1) : currentPreset.name;

    auto* alertWindow = new juce::AlertWindow("Save Preset",
                                              "Enter preset name:",
                                              juce::AlertWindow::NoIcon);
    alertWindow->setLookAndFeel(&getLookAndFeel());

    alertWindow->addTextEditor("presetName", defaultName, "Preset Name:");
    alertWindow->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
    alertWindow->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    alertWindow->enterModalState(true, juce::ModalCallbackFunction::create([this, currentIndex, alertWindow](int result)
    {
        if (result == 1)
        {
            juce::String presetName = alertWindow->getTextEditorContents("presetName");

            if (presetName.isEmpty())
            {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "Invalid Name",
                    "Preset name cannot be empty."
                );
                delete alertWindow;
                return;
            }

            auto& sessionManager = processorRef.getSessionManager();
            Preset newPreset = processorRef.getPresetManager().getCurrentStateAsPreset(presetName);
            sessionManager.setPreset(currentIndex, newPreset);

            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::InfoIcon,
                "Preset Saved",
                "Preset \"" + presetName + "\" saved to slot " + juce::String(currentIndex + 1)
            );
        }
        delete alertWindow;
    }), true);
}

void PluginEditor::handleReloadPreset()
{
    auto& sessionManager = processorRef.getSessionManager();
    int currentIndex = sessionManager.getCurrentPresetIndex();

    if (currentIndex < 0 || currentIndex >= SessionManager::MAX_PRESETS)
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "No Preset Selected",
            "Please select a preset slot first to reload its saved state."
        );
        return;
    }

    const auto& preset = sessionManager.getPreset(currentIndex);

    if (preset.isEmpty)
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "Empty Preset",
            "Cannot reload an empty preset slot."
        );
        return;
    }

    if (!preset.sourceFile.existsAsFile())
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "File Not Found",
            "Cannot reload preset - the source file no longer exists."
        );
        return;
    }

    Preset reloadedPreset;
    if (processorRef.getPresetManager().loadPreset(preset.sourceFile, reloadedPreset))
    {
        processorRef.getPresetManager().applyPreset(reloadedPreset);
        sessionManager.setPreset(currentIndex, reloadedPreset);
    }
    else
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "Reload Failed",
            "Failed to reload preset from file."
        );
    }
}

void PluginEditor::handleNewCollection()
{
    auto& sessionManager = processorRef.getSessionManager();

    if (!sessionManager.hasRootFolder())
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "No Root Folder",
            "Please select a root folder first using the folder icon."
        );
        return;
    }

    auto* alertWindow = new juce::AlertWindow("New Collection",
                                              "Enter collection name:",
                                              juce::AlertWindow::NoIcon);
    alertWindow->setLookAndFeel(&getLookAndFeel());

    alertWindow->addTextEditor("collectionName", "", "Collection Name:");
    alertWindow->addButton("Create", 1, juce::KeyPress(juce::KeyPress::returnKey));
    alertWindow->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    alertWindow->enterModalState(true, juce::ModalCallbackFunction::create([this, alertWindow](int result)
    {
        if (result == 1)
        {
            juce::String collectionName = alertWindow->getTextEditorContents("collectionName").trim();

            if (collectionName.isEmpty())
            {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "Invalid Name",
                    "Collection name cannot be empty."
                );
                delete alertWindow;
                return;
            }

            if (processorRef.createCollection(collectionName))
            {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::InfoIcon,
                    "Collection Created",
                    "Collection \"" + collectionName + "\" created and selected."
                );
            }
            else
            {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "Creation Failed",
                    "Could not create collection \"" + collectionName + "\". It may already exist."
                );
            }
        }
        delete alertWindow;
    }), true);
}

void PluginEditor::handleCollectionSelected(const juce::String& name)
{
    if (!processorRef.selectCollection(name))
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "Switch Failed",
            "Could not switch to collection \"" + name + "\"."
        );
    }
}

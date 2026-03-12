#include "session_name_display.h"

SessionNameDisplay::SessionNameDisplay(SessionManager& sm)
    : sessionManager(sm)
{
    sessionManager.addListener(this);

    collectionComboBox.setTextWhenNothingSelected("No Collection");
    collectionComboBox.setColour(juce::ComboBox::backgroundColourId, ColourCodes::bg2);
    collectionComboBox.setColour(juce::ComboBox::textColourId, ColourCodes::white0);
    collectionComboBox.setColour(juce::ComboBox::outlineColourId, ColourCodes::grey3);
    collectionComboBox.setColour(juce::ComboBox::arrowColourId, ColourCodes::white0);

    collectionComboBox.onChange = [this]()
    {
        auto selectedText = collectionComboBox.getText();
        if (selectedText.isNotEmpty() && onCollectionSelected)
            onCollectionSelected(selectedText);
    };

    addAndMakeVisible(collectionComboBox);
    updateCollectionList();
}

SessionNameDisplay::~SessionNameDisplay()
{
    sessionManager.removeListener(this);
}

void SessionNameDisplay::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::transparentBlack);
}

void SessionNameDisplay::resized()
{
    collectionComboBox.setBounds(getLocalBounds().reduced(2));
}

void SessionNameDisplay::sessionChanged()
{
    updateCollectionList();
    repaint();
}

void SessionNameDisplay::currentPresetChanged(int newIndex)
{
    juce::ignoreUnused(newIndex);
}

void SessionNameDisplay::updateCollectionList()
{
    collectionComboBox.clear(juce::dontSendNotification);

    auto collections = sessionManager.getCollectionNames();
    juce::String currentCollection = sessionManager.getCurrentCollectionName();

    int selectedId = 0;
    for (int i = 0; i < collections.size(); ++i)
    {
        int itemId = i + 1;
        collectionComboBox.addItem(collections[i], itemId);
        if (collections[i] == currentCollection)
            selectedId = itemId;
    }

    if (selectedId > 0)
        collectionComboBox.setSelectedId(selectedId, juce::dontSendNotification);
}

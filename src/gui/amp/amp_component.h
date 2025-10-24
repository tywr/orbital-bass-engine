#pragma once

#include "../colours.h"
#include "amp_knobs_component.h"
#include "amp_type.h"
#include "designs/helios.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class AmpComponent : public juce::Component
{
  public:
    AmpComponent(juce::AudioProcessorValueTreeState&);
    ~AmpComponent() override;

    void setColours(juce::Colour, juce::Colour);
    void initType();
    void switchType(AmpType);

    void resized() override;

    void paint(juce::Graphics&) override;
    void paintTypeButtons(juce::Graphics&);
    void paintDesign(juce::Graphics&, juce::Rectangle<float>);
    void paintBorder(juce::Graphics&, juce::Rectangle<float>, float);

  private:
    void buildCache(float scale);
    juce::Image background_cache;
    bool is_cache_dirty = true;

    juce::AudioProcessorValueTreeState& parameters;
    AmpKnobsComponent knobs_component;
    juce::ToggleButton bypass_button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypass_attachment;

    juce::Slider type_slider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        type_slider_attachment;

    juce::Colour current_colour1;
    juce::Colour current_colour2;

    AmpType helios_type = {
        &helios_button, "helios", ColourCodes::helios_yellow,
        ColourCodes::helios_orange
    };
    AmpType borealis_type = {
        &borealis_button, "borealis", ColourCodes::blue1, ColourCodes::blue2
    };
    AmpType nebula_type = {
        &nebula_button, "nebula", ColourCodes::nebula_violet,
        ColourCodes::nebula_red
    };

    std::vector<AmpType> types = {helios_type, borealis_type, nebula_type};

    HeliosToggleButton helios_button = HeliosToggleButton(helios_type);
    BorealisToggleButton borealis_button = BorealisToggleButton(borealis_type);
    NebulaToggleButton nebula_button = NebulaToggleButton(nebula_type);

    AmpType selected_type = types[0];

    // cache for paint
    std::vector<juce::Point<float>> voronoi_sites;
    std::vector<juce::Path> voronoi_cells;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpComponent)
};

#pragma once

#include "../components/labeled_knob.h"
#include "../components/solid_tooltip.h"
#include "amp_type.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

struct AmpKnob
{
    LabeledKnob* knob;
    juce::String parameter_id;
    juce::String label_text;
};

class AmpKnobsComponent : public juce::Component
{
  public:
    AmpKnobsComponent(juce::AudioProcessorValueTreeState&);
    ~AmpKnobsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void switchType();
    void switchColour(juce::Colour, juce::Colour);

  private:
    juce::AudioProcessorValueTreeState& parameters;
    SolidTooltip drag_tooltip;
    bool slider_being_dragged = false;

    LabeledKnob drive_knob;
    LabeledKnob level_knob;
    LabeledKnob era_knob;
    LabeledKnob grunt_knob;
    LabeledKnob attack_knob;
    LabeledKnob cross_frequency_knob;
    LabeledKnob bass_frequency_knob;
    LabeledKnob mix_knob;
    LabeledKnob master_knob;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;

    std::vector<AmpKnob> knobs = {
        {&drive_knob,           "overdrive_drive",         "drive"   },
        {&era_knob,             "overdrive_era",           "era"     },
        {&attack_knob,          "overdrive_attack",        "attack"  },
        {&grunt_knob,           "overdrive_grunt",         "grunt"   },
        {&bass_frequency_knob,  "overdrive_bass_frequency", "low pass"},
        {&cross_frequency_knob, "overdrive_x_frequency",   "hi pass" },
        {&level_knob,           "overdrive_level_db",      "level"   },
        {&mix_knob,             "overdrive_mix",           "mix"     },
        {&master_knob,          "amp_master",              "master"  },
    };
    std::vector<AmpKnob> current_knobs = {
        {&drive_knob,  "overdrive_drive",    "drive" },
        {&era_knob,    "overdrive_era",      "era"   },
        {&grunt_knob,  "overdrive_grunt",    "grunt" },
        {&attack_knob, "overdrive_attack",   "attack"},
        {&level_knob,  "overdrive_level_db", "level" },
        {&mix_knob,    "overdrive_mix",      "mix"   },
        {&master_knob, "amp_master",         "master"},
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpKnobsComponent)
};

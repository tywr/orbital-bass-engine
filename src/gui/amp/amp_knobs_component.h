#pragma once

#include "../components/solid_tooltip.h"
#include "amp_type.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <unordered_map>

struct AmpKnob
{
    juce::Slider* slider;
    juce::Label* label;
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
    void switchType(AmpType new_type);
    void switchColour(juce::Colour, juce::Colour);

  private:
    juce::AudioProcessorValueTreeState& parameters;
    SolidTooltip drag_tooltip;
    bool slider_being_dragged = false;

    juce::Slider drive_slider;
    juce::Label drive_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        drive_slider_attachment;

    juce::Slider level_slider;
    juce::Label level_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        level_slider_attachment;

    juce::Slider era_slider;
    juce::Label era_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        era_slider_attachment;

    juce::Slider attack_slider;
    juce::Label attack_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        attack_slider_attachment;

    juce::Slider cross_frequency_slider;
    juce::Label cross_frequency_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        cross_frequency_slider_attachment;

    juce::Slider bass_frequency_slider;
    juce::Label bass_frequency_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        bass_frequency_slider_attachment;

    juce::Slider mod_slider;
    juce::Label mod_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        mod_slider_attachment;

    juce::Slider aggro_slider;
    juce::Label aggro_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        aggro_slider_attachment;

    juce::Slider mix_slider;
    juce::Label mix_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        mix_slider_attachment;

    juce::Slider bass_slider;
    juce::Label bass_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        bass_slider_attachment;

    juce::Slider low_mid_slider;
    juce::Label low_mid_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        low_mid_slider_attachment;

    juce::Slider hi_mid_slider;
    juce::Label hi_mid_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        hi_mid_slider_attachment;

    juce::Slider treble_slider;
    juce::Label treble_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        treble_slider_attachment;

    juce::Slider master_slider;
    juce::Label master_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        master_slider_attachment;

    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        slider_attachments;

    std::vector<AmpKnob> knobs = {
        {&drive_slider,           &drive_label,           "overdrive_drive",    "drive"   },
        {&era_slider,             &era_label,             "overdrive_era",      "era"     },
        {&attack_slider,          &attack_label,          "overdrive_attack",   "attack"  },
        {&bass_frequency_slider,  &bass_frequency_label,
         "overdrive_bass_frequency",                                            "low pass"},
        {&cross_frequency_slider, &cross_frequency_label,
         "overdrive_x_frequency",                                               "hi pass" },
        {&mod_slider,             &mod_label,             "overdrive_mod",      "mod"     },
        {&aggro_slider,           &aggro_label,           "overdrive_aggro",    "aggro"   },
        {&level_slider,           &level_label,           "overdrive_level_db", "level"   },
        {&mix_slider,             &mix_label,             "overdrive_mix",      "mix"     },
        {&bass_slider,            &bass_label,            "amp_eq_bass",        "bass"    },
        {&low_mid_slider,         &low_mid_label,         "amp_eq_low_mid",     "lo mids" },
        {&hi_mid_slider,          &hi_mid_label,          "amp_eq_hi_mid",      "hi mids" },
        {&treble_slider,          &treble_label,          "amp_eq_treble",      "treble"  },
        {&master_slider,          &master_label,          "amp_master",         "master"  },
    };
    std::vector<AmpKnob> current_knobs = {
        {&drive_slider,   &drive_label,   "overdrive_drive",    "drive"  },
        {&era_slider,     &era_label,     "overdrive_era",      "era"    },
        {&attack_slider,  &attack_label,  "overdrive_attack",   "attack" },
        {&level_slider,   &level_label,   "overdrive_level_db", "level"  },
        {&mix_slider,     &mix_label,     "overdrive_mix",      "mix"    },
        {&bass_slider,    &bass_label,    "amp_eq_bass",        "bass"   },
        {&low_mid_slider, &low_mid_label, "amp_eq_low_mid",     "lo mids"},
        {&hi_mid_slider,  &hi_mid_label,  "amp_eq_hi_mid",      "hi mids"},
        {&treble_slider,  &treble_label,  "amp_eq_treble",      "treble" },
        {&master_slider,  &master_label,  "amp_master",         "master" },
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpKnobsComponent)
};

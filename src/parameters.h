#include "assets/ImpulseResponseBinaryMapping.h"
#include <juce_dsp/juce_dsp.h>

inline juce::AudioProcessorValueTreeState::ParameterLayout
createParameterLayout()
{
    return {
        std::make_unique<juce::AudioParameterFloat>(
            "input_gain_db", "Input Gain dB",
            juce::NormalisableRange<float>(-48.0f, 6.0f, 0.1f, 0.9f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "output_gain_db", "Output Gain dB",
            juce::NormalisableRange<float>(-48.0f, 6.0f, 0.1f, 0.9f), 0.0f
        ),
        std::make_unique<juce::AudioParameterChoice>(
            "compressor_type",                // Parameter ID
            "Compressor Type",                // Display name
            juce::StringArray{"OPTO", "FET"}, // Choice options
            0
        ),
        std::make_unique<juce::AudioParameterBool>(
            "compressor_bypass", "Compressor Bypass", false
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "compressor_threshold", "Compressor Treshold",
            juce::NormalisableRange<float>(-48.0f, 0.0f, 0.1f), -24.0f
        ),
        std::make_unique<juce::AudioParameterChoice>(
            "compressor_ratio", "Ratio",
            juce::StringArray{"2:1", "4:1", "8:1", "12:1", "20:1"},
            0 // Default index
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "compressor_level_db", "Compressor Gain dB",
            juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f, 1.0f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "compressor_mix", "Compressor Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f
        ),
        std::make_unique<juce::AudioParameterBool>(
            "fuzz_bypass", "Fuzz Bypass", false
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "fuzz_tone", "Fuzz Tone",
            juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f), 5.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "fuzz_sustain", "Fuzz Sustain",
            juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f), 5.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "fuzz_mix", "Fuzz Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "fuzz_level", "Fuzz Level",
            juce::NormalisableRange<float>(-24.0f, 12.0f, 0.01f), 5.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "amp_master", "Amp Master Level",
            juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f, 1.0f), 0.0f
        ),
        std::make_unique<juce::AudioParameterBool>(
            "amp_bypass", "Amp Bypass", false
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "overdrive_level_db", "Overdrive Level dB",
            juce::NormalisableRange<float>(-48.0f, 6.0f, 0.1f, 1.0f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "overdrive_drive", "Overdrive Drive",
            juce::NormalisableRange<float>(0.0, 10.0f, 0.1f, 1.0f), 5.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "overdrive_grunt", "Overdrive Grunt",
            juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f), 5.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "overdrive_attack", "Overdrive Attack",
            juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f), 5.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "overdrive_era", "Overdrive Era",
            juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f), 5.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "overdrive_mix", "Overdrive Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f
        ),
        std::make_unique<juce::AudioParameterBool>(
            "chorus_bypass", "Chorus Mix", false
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "chorus_mix", "Chorus Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "chorus_rate", "Chorus Rate",
            juce::NormalisableRange<float>(0.5f, 2.5f, 0.01f), 1.5f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "chorus_depth", "Chorus Depth",
            juce::NormalisableRange<float>(0.0f, 1.5f, 0.01f), 0.75f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "chorus_crossover", "Chorus Crossover",
            juce::NormalisableRange<float>(50.0f, 1000.0f, 0.1f, 0.3755212f),
            200.0f
        ),
        std::make_unique<juce::AudioParameterBool>(
            "ir_bypass", "Impulse Response Bypass", false
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "ir_mix", "Impulse Response Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f
        ),
        std::make_unique<juce::AudioParameterChoice>(
            "ir_type", "IR Type", impulseResponseBinaryNames, 0
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "ir_level", "Impulse Response Level",
            juce::NormalisableRange<float>(-36.0f, 12.0f, 0.1f, 1.0f), -18.0f
        ),
        std::make_unique<juce::AudioParameterBool>(
            "synth_bypass", "Synth Bypass", true
        ),
        std::make_unique<juce::AudioParameterBool>(
            "eq_bypass", "EQ Bypass", true
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "eq_b80", "EQ Low-band",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "eq_b250", "EQ 250 Hz-Band",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "eq_b500", "EQ 500 Hz-Band",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "eq_b1500", "EQ 1.5 kHz-Band",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "eq_b3000", "EQ 3 kHz-Band",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "eq_b5000", "EQ High-band",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "eq_lpf", "EQ LPF",
            juce::NormalisableRange<float>(1000.0f, 10000.0f, 1.0f), 3000.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "synth_octave_level", "Synth Voice Octave Level",
            juce::NormalisableRange<float>(-48.0f, 6.0f, 0.1f, 1.0f), -12.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "synth_square_level", "Synth Voice Square Level",
            juce::NormalisableRange<float>(-48.0f, 6.0f, 0.1f, 1.0f), -12.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "synth_triangle_level", "Synth Voice Triangle Level",
            juce::NormalisableRange<float>(-48.0f, 6.0f, 0.1f, 1.0f), -12.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "synth_raw_level", "Synth Voice Raw Level",
            juce::NormalisableRange<float>(-48.0f, 6.0f, 0.1f, 1.0f), -12.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "synth_master_level", "Synth Voice Master Level",
            juce::NormalisableRange<float>(-48.0f, 6.0f, 0.1f, 1.0f), -12.0f
        ),
    };
}

#include <juce_dsp/juce_dsp.h>

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
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
            "compressor_type",                       // Parameter ID
            "Compressor Type",                       // Display name
            juce::StringArray{"OPTO", "FET", "VCA"}, // Choice options
            0
        ),
        std::make_unique<juce::AudioParameterBool>(
            "compressor_bypass", "Compressor Bypass", false
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "compressor_threshold", "Compressor Treshold",
            juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f, 3.0f), -12.0f
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
            juce::NormalisableRange<float>(0, 100, 1, 1.0f), 50
        ),
        std::make_unique<juce::AudioParameterChoice>(
            "amp_type",                                        // Parameter ID
            "Amp Type",                                        // Display name
            juce::StringArray{"helios", "borealis", "nebula"}, // Choice options
            0
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
            juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f, 1.0f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "overdrive_drive", "Overdrive Drive",
            juce::NormalisableRange<float>(0.0, 10.0f, 0.1f, 1.0f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "overdrive_attack", "Overdrive Attack",
            juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f), 5.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "overdrive_grunt", "Overdrive Grunt",
            juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f), 5.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "overdrive_x", "Overdrive Crossover",
            juce::NormalisableRange<float>(250.0f, 1000.0f, 1.0f, 0.6309f),
            500.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "overdrive_x_level", "Overdrive X Level",
            juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f), 5.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "overdrive_mod", "Overdrive Mod",
            juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f), 5.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "overdrive_aggro", "Overdrive Aggro",
            juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f), 5.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "overdrive_mix", "Overdrive Mix",
            juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "amp_eq_bass", "Amp EQ Bass",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f, 1.0f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "amp_eq_low_mid", "Amp EQ Lo-Mids",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f, 1.0f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "amp_eq_hi_mid", "Amp EQ Hi-Mids",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f, 1.0f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "amp_eq_treble", "Amp EQ Treble",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f, 1.0f), 0.0f
        ),
        std::make_unique<juce::AudioParameterBool>(
            "ir_bypass", "Impulse Response Bypass", false
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "ir_mix", "Impulse Response Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "ir_gain_db", "Impulse Response Gain dB",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f, 1.0f), 0.0f
        )
    };
}

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
            juce::NormalisableRange<float>(-48.0f, 12.0f, 0.1f, 0.9f), 0.0f
        ),
        std::make_unique<juce::AudioParameterBool>(
            "compressor_bypass", "Compressor Bypass", false
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "compressor_hpf", "Compressor HPF (Hz)",
            juce::NormalisableRange<float>(
                20.0f, 200.0f, 0.1f, 0.2890647108933747f
            ),
            20.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "compressor_threshold", "Compressor Treshold",
            juce::NormalisableRange<float>(-48.0f, 0.0f, 0.1f), -24.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "compressor_ratio", "Compressor Ratio",
            juce::NormalisableRange<float>(2.0f, 20.0f, 0.1f), 4.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "compressor_attack", "Compressor Attack Time (ms)",
            juce::NormalisableRange<float>(
                0.1f, 100.0f, 0.01f, 0.2890647108933747f
            ),
            10.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "compressor_release", "Compressor Release Time (ms)",
            juce::NormalisableRange<float>(
                10.0f, 1000.0f, 0.1f, 0.2890647108933747f
            ),
            100.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "compressor_level_db", "Compressor Gain dB",
            juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f, 1.0f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "compressor_mix", "Compressor Mix",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f
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
            "chorus_rate", "Chorus Rate (Hz)",
            juce::NormalisableRange<float>(0.5f, 2.5f, 0.01f), 1.5f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "chorus_depth", "Chorus Depth (ms)",
            juce::NormalisableRange<float>(0.0f, 1.5f, 0.01f), 0.75f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "chorus_crossover", "Chorus Crossover (Hz)",
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
            "eq_low_shelf_gain", "EQ Low Shelf Gain",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "eq_low_mid_freq", "EQ Low-Mid Frequency",
            juce::NormalisableRange<float>(200.0f, 800.0f, 1.0f), 400.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "eq_low_mid_q", "EQ Low-Mid Q",
            juce::NormalisableRange<float>(0.1f, 4.0f, 0.01f), 0.7f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "eq_low_mid_gain", "EQ Low-Mid Gain",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "eq_high_mid_freq", "EQ High-Mid Frequency",
            juce::NormalisableRange<float>(800.0f, 2500.0f, 1.0f), 1500.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "eq_high_mid_q", "EQ High-Mid Q",
            juce::NormalisableRange<float>(0.1f, 4.0f, 0.01f), 0.7f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "eq_high_mid_gain", "EQ High-Mid Gain",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.01f), 0.0f
        ),
        std::make_unique<juce::AudioParameterFloat>(
            "eq_high_shelf_gain", "EQ High Shelf Gain",
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

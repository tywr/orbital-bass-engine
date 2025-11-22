#include "plugin_audio_processor.h"

void PluginAudioProcessor::setParameterValue(juce::String parameterID, float v)
{
    if (parameterID == "compressor_attack")
    {
        compressor.setAttack(v);
    }
    else if (parameterID == "compressor_release")
    {
        compressor.setRelease(v);
    }
    else if (parameterID == "compressor_ratio")
    {
        compressor.setRatio(v);
    }
    else if (parameterID == "compressor_threshold")
    {
        compressor.setThresholdDecibels(v);
    }
    else if (parameterID == "compressor_level_db")
    {
        compressor.setLevel(juce::Decibels::decibelsToGain(v));
    }
    else if (parameterID == "compressor_mix")
    {
        float bv = juce::jlimit(0.0f, 1.0f, v);
        compressor.setMix(bv);
    }
    // Overdrive
    else if (parameterID == "overdrive_mix")
    {
        float bv = juce::jlimit(0.0f, 1.0f, v);
        overdrive.setMix(bv);
    }
    else if (parameterID == "overdrive_level_db")
    {
        overdrive.setLevel(juce::Decibels::decibelsToGain(v));
    }
    else if (parameterID == "overdrive_drive")
    {
        float bv = juce::jlimit(0.0f, 10.0f, v);
        overdrive.setDrive(bv);
    }
    else if (parameterID == "overdrive_attack")
    {
        overdrive.setAttack(v);
    }
    else if (parameterID == "overdrive_era")
    {
        overdrive.setEra(v);
    }
    else if (parameterID == "overdrive_grunt")
    {
        overdrive.setGrunt(v);
    }
    // EQ
    else if (parameterID == "eq_low_shelf_gain")
    {
        float bv = juce::jlimit(-20.0f, 20.0f, v);
        float dbv = juce::Decibels::decibelsToGain(bv);
        eq.setLowShelfGain(dbv);
    }
    else if (parameterID == "eq_low_mid_freq")
    {
        float bv = juce::jlimit(200.0f, 800.0f, v);
        eq.setLowMidFreq(bv);
    }
    else if (parameterID == "eq_low_mid_q")
    {
        float bv = juce::jlimit(0.1f, 4.0f, v);
        eq.setLowMidQ(bv);
    }
    else if (parameterID == "eq_low_mid_gain")
    {
        float bv = juce::jlimit(-20.0f, 20.0f, v);
        float dbv = juce::Decibels::decibelsToGain(bv);
        eq.setLowMidGain(dbv);
    }
    else if (parameterID == "eq_high_mid_freq")
    {
        float bv = juce::jlimit(800.0f, 2500.0f, v);
        eq.setHighMidFreq(bv);
    }
    else if (parameterID == "eq_high_mid_q")
    {
        float bv = juce::jlimit(0.1f, 4.0f, v);
        eq.setHighMidQ(bv);
    }
    else if (parameterID == "eq_high_mid_gain")
    {
        float bv = juce::jlimit(-20.0f, 20.0f, v);
        float dbv = juce::Decibels::decibelsToGain(bv);
        eq.setHighMidGain(dbv);
    }
    else if (parameterID == "eq_high_shelf_gain")
    {
        float bv = juce::jlimit(-20.0f, 20.0f, v);
        float dbv = juce::Decibels::decibelsToGain(bv);
        eq.setHighShelfGain(dbv);
    }
    else if (parameterID == "eq_lpf")
    {
        float bv = juce::jlimit(1000.0f, 10000.0f, v);
        eq.setLpfFrequency(bv);
    }
    // Chorus
    else if (parameterID == "chorus_mix")
    {
        float bv = juce::jlimit(0.0f, 1.0f, v);
        chorus.setMix(bv);
    }
    else if (parameterID == "chorus_rate")
    {
        float bv = juce::jlimit(0.0f, 5.0f, v);
        chorus.setRate(bv);
    }
    else if (parameterID == "chorus_crossover")
    {
        float bv = juce::jlimit(50.0f, 10000.0f, v);
        chorus.setCrossover(bv);
    }
    else if (parameterID == "chorus_depth")
    {
        float bv = juce::jlimit(0.0f, 6.0f, v);
        chorus.setDepth(bv);
    }
    // Impulse Response Convolver
    else if (parameterID == "ir_mix")
    {
        float bv = juce::jlimit(0.0f, 1.0f, v);
        irConvolver.setMix(bv);
    }
    else if (parameterID == "ir_level")
    {
        irConvolver.setLevel(juce::Decibels::decibelsToGain(v));
    }
    else if (parameterID == "ir_type")
    {
        irConvolver.setTypeFromIndex(static_cast<int>(v));
    }
    else if (parameterID == "synth_octave_level")
    {
        float dbv =
            juce::Decibels::decibelsToGain(juce::jlimit(-100.0f, 6.0f, v));
        synth_voices.setOctaveLevel(dbv);
    }
    else if (parameterID == "synth_square_level")
    {
        float dbv =
            juce::Decibels::decibelsToGain(juce::jlimit(-100.0f, 6.0f, v));
        synth_voices.setSquareLevel(dbv);
    }
    else if (parameterID == "synth_triangle_level")
    {
        float dbv =
            juce::Decibels::decibelsToGain(juce::jlimit(-100.0f, 6.0f, v));
        synth_voices.setTriangleLevel(dbv);
    }
    else if (parameterID == "synth_raw_level")
    {
        float dbv =
            juce::Decibels::decibelsToGain(juce::jlimit(-100.0f, 6.0f, v));
        synth_voices.setRawLevel(dbv);
    }
    else if (parameterID == "synth_master_level")
    {
        float dbv =
            juce::Decibels::decibelsToGain(juce::jlimit(-100.0f, 6.0f, v));
        synth_voices.setMasterLevel(dbv);
    }
}

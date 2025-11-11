#include "plugin_audio_processor.h"

void PluginAudioProcessor::setParameterValue(juce::String parameterID, float v)
{
    if (parameterID == "compressor_ratio")
    {
        const float values[] = {2.0f, 4.0f, 8.0f, 12.0f, 20.0f};
        int index = static_cast<int>(v);
        compressor.setRatio(values[index]);
    }
    else if (parameterID == "compressor_threshold")
    {
        compressor.setThresholdDecibels(v);
    }
    else if (parameterID == "compressor_level_db")
    {
        compressor.setLevel(juce::Decibels::decibelsToGain(v));
    }
    else if (parameterID == "compressor_type")
    {
        compressor.setTypeFromIndex(static_cast<int>(v));
    }
    else if (parameterID == "compressor_mix")
    {
        float bv = juce::jlimit(0.0f, 1.0f, v);
        compressor.setMix(bv);
    }
    else if (parameterID == "fuzz_tone")
    {
        float bv = juce::jlimit(0.0f, 10.0f, v);
        fuzz.setTone(bv);
    }
    else if (parameterID == "fuzz_sustain")
    {
        float bv = juce::jlimit(0.0f, 10.0f, v);
        fuzz.setSustain(bv);
    }
    else if (parameterID == "fuzz_mix")
    {
        float bv = juce::jlimit(0.0f, 1.0f, v);
        fuzz.setMix(bv);
    }
    else if (parameterID == "fuzz_level")
    {
        float bv = juce::jlimit(-48.0f, 6.0f, v);
        fuzz.setLevel(juce::Decibels::decibelsToGain(bv));
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
        float dbv = juce::Decibels::decibelsToGain(juce::jlimit(-100.0f, 6.0f, v));
        synth_voices.setOctaveLevel(dbv);
    }
    else if (parameterID == "synth_square_level")
    {
        float dbv = juce::Decibels::decibelsToGain(juce::jlimit(-100.0f, 6.0f, v));
        synth_voices.setSquareLevel(dbv);
    }
    else if (parameterID == "synth_triangle_level")
    {
        float dbv = juce::Decibels::decibelsToGain(juce::jlimit(-100.0f, 6.0f, v));
        synth_voices.setTriangleLevel(dbv);
    }
    else if (parameterID == "synth_raw_level")
    {
        float dbv = juce::Decibels::decibelsToGain(juce::jlimit(-100.0f, 6.0f, v));
        synth_voices.setRawLevel(dbv);
    }
    else if (parameterID == "synth_master_level")
    {
        float dbv = juce::Decibels::decibelsToGain(juce::jlimit(-100.0f, 6.0f, v));
        synth_voices.setMasterLevel(dbv);
    }
}

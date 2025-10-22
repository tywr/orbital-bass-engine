#include "plugin_audio_processor.h"

void PluginAudioProcessor::setParameterValue(juce::String parameterID, float v)
{
    if (parameterID == "compressor_bypass")
    {
        compressor.setBypass(v >= 0.5f);
    }
    else if (parameterID == "compressor_ratio")
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
        compressor.setMix(static_cast<int>(v));
    }
    else if (parameterID == "amp_type")
    {
        int index = juce::jlimit(0, (int)overdrives.size() - 1, (int)v);
        Overdrive* next = overdrives[index];

        if (next != current_overdrive.load())
        {
            next->reset(); // reset buffers/envelopes
            current_overdrive.store(next);
        }
    }
    // Overdrive
    if (parameterID == "amp_bypass")
    {
        isAmpBypassed = (v >= 0.5f);
        amp_eq.setBypass(v >= 0.5f);
        for (auto& overdrive : overdrives)
        {
            overdrive->setBypass(v >= 0.5f);
        }
    }
    else if (parameterID == "overdrive_mix")
    {
        float bv = juce::jlimit(0.0f, 1.0f, v);
        for (auto& overdrive : overdrives)
        {
            overdrive->setMix(bv);
        }
    }
    else if (parameterID == "overdrive_level_db")
    {
        for (auto& overdrive : overdrives)
        {
            overdrive->setLevel(juce::Decibels::decibelsToGain(v));
        }
    }
    else if (parameterID == "overdrive_drive")
    {
        for (auto& overdrive : overdrives)
        {
            float bv = juce::jlimit(0.0f, 10.0f, v);
            overdrive->setDrive(bv);
        }
    }
    else if (parameterID == "overdrive_attack")
    {
        for (auto& overdrive : overdrives)
        {
            overdrive->setAttack(v);
        }
    }
    else if (parameterID == "overdrive_era")
    {
        for (auto& overdrive : overdrives)
        {
            overdrive->setEra(v);
        }
    }
    else if (parameterID == "overdrive_x_frequency")
    {
        float bv = juce::jlimit(20.0f, 20000.0f, v);
        for (auto& overdrive : overdrives)
        {
            overdrive->setCrossFrequency(bv);
        }
    }
    else if (parameterID == "overdrive_bass_frequency")
    {
        float bv = juce::jlimit(20.0f, 20000.0f, v);
        for (auto& overdrive : overdrives)
        {
            overdrive->setBassFrequency(bv);
        }
    }
    else if (parameterID == "overdrive_mod")
    {
        for (auto& overdrive : overdrives)
        {
            overdrive->setMod(v);
        }
    }
    else if (parameterID == "overdrive_aggro")
    {
        for (auto& overdrive : overdrives)
        {
            overdrive->setAggro(v);
        }
    }
    // Amp EQ
    else if (parameterID == "amp_eq_bass")
    {
        amp_eq.setBassGain(juce::Decibels::decibelsToGain(v));
    }
    else if (parameterID == "amp_eq_low_mid")
    {
        amp_eq.setLowMidGain(juce::Decibels::decibelsToGain(v));
    }
    else if (parameterID == "amp_eq_hi_mid")
    {
        amp_eq.setHighMidGain(juce::Decibels::decibelsToGain(v));
    }
    else if (parameterID == "amp_eq_treble")
    {
        amp_eq.setTrebleGain(juce::Decibels::decibelsToGain(v));
    }
    // Impulse Response Convolver
    else if (parameterID == "ir_bypass")
    {
        irConvolver.setBypass((v >= 0.5f) ? true : false);
    }
    else if (parameterID == "ir_mix")
    {
        float bv = juce::jlimit(0.0f, 1.0f, v);
        irConvolver.setMix(v);
    }
    else if (parameterID == "ir_level")
    {
        irConvolver.setLevel(juce::Decibels::decibelsToGain(v));
    }
    else if (parameterID == "ir_filepath")
    {
        // Load IR from the new filepath
        juce::String newFilepath =
            parameters.state.getProperty("ir_filepath").toString();
        irConvolver.setFilepath(newFilepath);
        irConvolver.loadIR();
    }
}

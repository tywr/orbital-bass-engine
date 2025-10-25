#pragma once
#include <juce_dsp/juce_dsp.h>

class Overdrive
{
  public:
    virtual void prepare(const juce::dsp::ProcessSpec& spec)
    {
        juce::ignoreUnused(spec);
    }
    virtual void process(juce::AudioBuffer<float>& buffer)
    {
        juce::ignoreUnused(buffer);
    }
    virtual void reset()
    {
    }
    void virtual setAttack(float newAttack)
    {
        float v = juce::jlimit(0.0f, 10.0f, newAttack);
        attack.setTargetValue(v);
        raw_attack = v;
    }
    void virtual setEra(float newEra)
    {
        float v = juce::jlimit(0.0f, 10.0f, newEra);
        era.setTargetValue(v);
        raw_era = v;
    }
    void virtual setCrossFrequency(float new_cross_frequency)
    {
        float v = juce::jlimit(250.0f, 1000.0f, new_cross_frequency);
        cross_frequency.setTargetValue(v);
        raw_cross_frequency = v;
    }
    void virtual setBassFrequency(float new_high_level)
    {
        float v = juce::jlimit(50.0f, 500.0f, new_high_level);
        bass_frequency.setTargetValue(v);
        raw_bass_frequency = v;
    }

    void applyGain(
        juce::AudioBuffer<float>& buffer, float& previous_gain, float& gain
    )
    {
        if (juce::approximatelyEqual(gain, previous_gain))
        {
            buffer.applyGain(gain);
        }
        else
        {
            buffer.applyGainRamp(
                0, buffer.getNumSamples(), previous_gain, gain
            );
            previous_gain = gain;
        }
    }
    void setBypass(bool shouldBypass)
    {
        bypass = shouldBypass;
    }
    void setLevel(float newLevel)
    {
        float v = juce::jlimit(0.0f, 10.0f, newLevel);
        level.setTargetValue(v);
        raw_level = v;
    }
    void setMix(float newMix)
    {
        float v = juce::jlimit(0.0f, 1.0f, newMix);
        mix.setTargetValue(v);
        raw_mix = v;
    }
    void setDrive(float newDrive)
    {
        float v = juce::jlimit(0.0f, 10.0f, newDrive);
        drive.setTargetValue(v);
        raw_drive = v;
    }

  protected:
    juce::dsp::ProcessSpec processSpec{-1, 0, 0};
    float smoothing_time = 0.05f;

    // gui parameters
    int type;
    bool bypass;
    float raw_level, raw_drive, raw_mix, raw_attack, raw_era,
        raw_cross_frequency, raw_bass_frequency, raw_mod, raw_aggro;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> level, drive,
        mix, attack, era, cross_frequency, bass_frequency, mod, aggro;
};

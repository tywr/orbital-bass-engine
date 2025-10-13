#pragma once
#include <juce_dsp/juce_dsp.h>

class Overdrive
{
  public:
    virtual void prepare(const juce::dsp::ProcessSpec& spec)
    {
        juce::ignoreUnused(spec);
    }
    virtual float driveToGain(float d)
    {
        float t = d / 10.0f;
        return juce::Decibels::decibelsToGain(t * 24.0f);
    }
    virtual void process(juce::AudioBuffer<float>& buffer)
    {
        juce::ignoreUnused(buffer);
    }
    void virtual setAttack(float newAttack)
    {
        attack = newAttack;
    }
    void virtual setGrunt(float newGrunt)
    {
        grunt = newGrunt;
    }
    void virtual setCrossFrequency(float new_cross_frequency)
    {
        cross_frequency = new_cross_frequency;
    }
    void virtual setHighLevel(float new_high_level)
    {
        high_level = new_high_level;
    }
    void virtual setMod(float new_mod)
    {
        mod = new_mod;
    }
    void virtual setAggro(float new_aggro)
    {
        aggro = new_aggro;
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
        level = newLevel;
    }
    void setMix(float newMix)
    {
        mix = newMix;
    }
    void setDrive(float newDrive)
    {
        drive = newDrive;
    }

  protected:
    juce::dsp::ProcessSpec processSpec{-1, 0, 0};

    // gui parameters
    int type;
    bool bypass;
    float level;
    float drive;
    float mix;
    float attack;
    float grunt;
    float cross_frequency;
    float high_level;
    float mod;
    float aggro;

    // state parameters
    float previous_drive_gain = 1.0f;
    float previous_level = 1.0f;
};

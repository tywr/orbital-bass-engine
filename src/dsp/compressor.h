#pragma once

#include "circuits/jfet.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class Compressor : juce::dsp::ProcessorBase
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(
        const juce::dsp::ProcessContextReplacing<float>& context
    ) override;
    void reset() override;
    void computeGainReductionOptometric(float& sample, float sampleRate);
    void computeGainReductionFet(float& sample, float sampleRate);
    void computeGainReductionVca(float& sample, float sampleRate);
    void applyLevel(juce::AudioBuffer<float>& buffer);

    void setRatio(float newRatio)
    {
        float v = juce::jlimit(0.0f, 20.0f, newRatio);
        ratio.setTargetValue(v);
        raw_ratio = v;
    }
    void setMix(float newMix)
    {
        float v = juce::jlimit(0.0f, 1.0f, newMix);
        mix.setTargetValue(v);
        raw_mix = v;
    }
    void setThresholdDecibels(float newThreshold)
    {
        float v = juce::jlimit(-60.0f, 6.0f, newThreshold);
        threshold_db.setTargetValue(v);
        raw_threshold_db = v;
    }

    void setLevel(float newLevel)
    {
        float v = juce::jlimit(0.0f, 10.0f, newLevel);
        level.setTargetValue(v);
        raw_level = v;
    }

    void setTypeFromIndex(int index)
    {
        // 0 = OPTO, 1 = FET, 2 = VCA
        type = index;
    }

    float getGainReductionDb()
    {
        return gain_smooth_db;
    }

  private:
    juce::dsp::ProcessSpec processSpec{-1, 0, 0};
    int debugCounter = 0;

    void (Compressor::*gainFunction)(float&, float) = nullptr;

    // gui parameters
    int type;
    int lastType = -1;

    float smoothing_time = 0.05f;
    float raw_mix, raw_level, raw_threshold_db, raw_ratio;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mix, level,
        threshold_db, ratio;

    // internal state of compressor
    float current_level = 1.0f;
    float current_level_db = 1.0f;
    float gain_smooth_db = 0.0f;
    float gain_smooth = 1.0f;

    // circuits
    JFET jfet = JFET(0.5f, 0.15f, 0.9f);

    // hardcoded parameters for optometric compressor
    struct
    {
        float attack = 0.01f;
        float release1 = 0.06f;
        float release2 = 0.5f;
        float gainSmoothingTime = 0.05f;
        float gr_to_sat = 0.1f;
        float saturation = 0.1f;
    } optoParams;

    struct
    {
        float attack = 0.0003f;
        float release = 0.1f;
        float gr_to_sat = 0.1f;
        float saturation = 0.15f;
    } fetParams;

    struct
    {
        float attack = 0.005f;
        float release = 0.4f;
        float gainSmoothingTime = 0.01f;
        float kneeWidth = 2.0f;
    } vcaParams;
};

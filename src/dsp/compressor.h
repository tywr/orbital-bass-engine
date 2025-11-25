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
    void updateHPF();
    void applyLevel(juce::AudioBuffer<float>& buffer);

    void setHPF(float newHPF)
    {
        float v = juce::jlimit(0.0f, 20000.0f, newHPF);
        hpf_freq.setTargetValue(v);
        raw_hpf_freq = v;
    }
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

    void setAttack(float newAttack)
    {
        float v = juce::jlimit(0.0f, 1000.0f, newAttack);
        attack.setTargetValue(v);
        raw_attack = v;
    }

    void setRelease(float newRelease)
    {
        float v = juce::jlimit(0.0f, 10000.0f, newRelease);
        release.setTargetValue(v);
        raw_release = v;
    }

    float getGainReductionDb()
    {
        return gr_db;
    }

  private:
    juce::dsp::ProcessSpec processSpec{-1, 0, 0};
    int debugCounter = 0;

    void (Compressor::*gainFunction)(float&, float) = nullptr;
    juce::dsp::IIR::Filter<float> hpf_filter;

    // gui parameters
    int type;
    int lastType = -1;

    float smoothing_time = 0.05f;
    float raw_mix = 1.0f;
    float raw_level = 1.0f;
    float raw_threshold_db = 0.0f;
    float raw_ratio = 1.0f;
    float raw_attack = 5.0f;
    float raw_release = 50.0f;
    float raw_hpf_freq = 20.0f;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mix, level,
        threshold_db, ratio, attack, release, hpf_freq;

    // internal state of compressor
    float current_level = 1.0f;
    float current_level_db = 1.0f;
    float gr_db = 0.0f;
    float gr = 1.0f;
    float width = 6.0f;
    // float attack = 0.0003f;
    // float release = 0.1f;
};

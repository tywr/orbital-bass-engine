#include "compressor.h"

#include <juce_dsp/juce_dsp.h>

void Compressor::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
}

void Compressor::computeGainReductionOptometric(float& sample, float sampleRate)
{
    // Optometric Compressor
    //
    // Envelope processing
    float input_level = std::abs(sample);
    float input_level_db = juce::Decibels::gainToDecibels(input_level + 1e-10f);
    float current_threshold_db = threshold_db.getCurrentValue();
    float current_ratio = ratio.getCurrentValue();

    float coef;
    if (input_level_db > current_level_db)
    {
        coef = std::exp(std::log(0.01f) / (sampleRate * optoParams.attack));
    }
    else
    {
        if (current_level_db > current_threshold_db)
        {
            coef =
                std::exp(std::log(0.01f) / (sampleRate * optoParams.release1));
        }
        else
        {
            coef =
                std::exp(std::log(0.01f) / (sampleRate * optoParams.release2));
        }
    }
    current_level_db = input_level_db;
    current_level = juce::Decibels::decibelsToGain(current_level_db);

    // Gain Computation
    float target_gain_db;
    if (current_level_db > current_threshold_db)
    {
        target_gain_db = (current_threshold_db - input_level_db) *
                         (1.0f - 1.0f / current_ratio);
    }
    else
    {
        target_gain_db = 0.0f;
    }

    gain_smooth_db = (coef * gain_smooth_db) + ((1.0f - coef) * target_gain_db);
    gain_smooth = juce::Decibels::decibelsToGain(gain_smooth_db);
    sample = sample * gain_smooth;
}

void Compressor::computeGainReductionFet(float& sample, float sampleRate)
{
    float output = sample;
    float output_level =
        std::max(std::abs(gain_smooth * output), current_level * 0.99f);
    float output_level_db =
        juce::Decibels::gainToDecibels(output_level + 1e-10f);
    float current_threshold_db = threshold_db.getCurrentValue();
    float current_ratio = static_cast<float>(ratio.getCurrentValue());

    float target_gain_db = 0.0f;
    if (output_level_db > current_threshold_db)
    {
        target_gain_db = (current_threshold_db - output_level_db) *
                         (1.0f - 1.0f / current_ratio);
    }

    float coef;
    if (target_gain_db < gain_smooth_db)
    {
        coef = std::exp(-1.0f / (sampleRate * fetParams.attack));
    }
    else
    {
        coef = std::exp(-1.0f / (sampleRate * fetParams.release));
    }

    gain_smooth_db = (coef * gain_smooth_db) + ((1.0f - coef) * target_gain_db);
    gain_smooth = juce::Decibels::decibelsToGain(gain_smooth_db);

    sample = sample * gain_smooth;
    current_level = sample;
    current_level_db =
        juce::Decibels::gainToDecibels(std::abs(current_level) + 1e-10f);
}

void Compressor::computeGainReductionVca(float& sample, float sampleRate)
{
    float input_level = std::abs(sample);
    float input_level_db = juce::Decibels::gainToDecibels(input_level + 1e-10f);
    float current_threshold_db = threshold_db.getCurrentValue();
    float current_ratio = ratio.getCurrentValue();

    static float rms_buffer[64] = {0};
    static int rms_index = 0;

    rms_buffer[rms_index] = input_level * input_level;
    rms_index = (rms_index + 1) % 64;

    float rms_sum = 0.0f;
    for (int i = 0; i < 64; ++i)
        rms_sum += rms_buffer[i];

    float rms_level = std::sqrt(rms_sum / 64.0f);
    float rms_level_db = juce::Decibels::gainToDecibels(rms_level + 1e-10f);

    float coef;
    if (rms_level_db > current_level_db)
    {
        coef = std::exp(std::log(0.01) / (sampleRate * vcaParams.attack));
    }
    else
    {
        coef = std::exp(std::log(0.01) / (sampleRate * vcaParams.release));
    }
    current_level_db =
        (coef * current_level_db) + ((1.0f - coef) * rms_level_db);

    float target_gain_db;
    float rawGainReductionDb;
    if (current_level_db > current_threshold_db)
    {
        float over_threshold = current_level_db - current_threshold_db;
        target_gain_db = -over_threshold * (1.0f - 1.0f / current_ratio);

        if (over_threshold < vcaParams.kneeWidth)
        {
            float kneeRatio = over_threshold / vcaParams.kneeWidth;
            target_gain_db *= (kneeRatio * kneeRatio);
        }
    }
    else
    {
        target_gain_db = 0.0f;
    }

    float target_gain = juce::Decibels::decibelsToGain(target_gain_db);

    gain_smooth_db = target_gain_db;
    gain_smooth = juce::Decibels::decibelsToGain(gain_smooth_db);

    sample = sample * gain_smooth;
}

void Compressor::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        gain_smooth_db = 0.0f;
        return;
    }
    float sampleRate = static_cast<float>(processSpec.sampleRate);

    auto* channelData = buffer.getWritePointer(0);
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        if (threshold_db.isSmoothing())
            threshold_db.skip(1);

        if (ratio.isSmoothing())
            ratio.skip(1);

        if (mix.isSmoothing())
            mix.skip(1);

        if (level.isSmoothing())
            level.skip(1);

        float wet = channelData[sample];
        float dry = channelData[sample];

        switch (type)
        {
        case 0:
            computeGainReductionOptometric(wet, sampleRate);
            break;
        case 1:
            computeGainReductionFet(wet, sampleRate);
            break;
        case 2:
            computeGainReductionVca(wet, sampleRate);
            break;
        }

        float current_mix = mix.getCurrentValue();
        float current_level = level.getCurrentValue();

        channelData[sample] =
            (dry * (1.0f - current_mix) + wet * current_mix * current_level);
    }
}

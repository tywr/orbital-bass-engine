#include "compressor.h"

#include <algorithm>
#include <cmath>
#include <juce_dsp/juce_dsp.h>

void Compressor::reset()
{
    current_level = 1.0f;
    current_level_db = 0.0f;
    gain_smooth = 1.0f;
    gain_smooth_db = 0.0f;

    mix.reset(processSpec.sampleRate, smoothing_time);
    mix.setCurrentAndTargetValue(raw_mix);
    level.reset(processSpec.sampleRate, smoothing_time);
    level.setCurrentAndTargetValue(raw_level);
    threshold_db.reset(processSpec.sampleRate, smoothing_time);
    threshold_db.setCurrentAndTargetValue(raw_threshold_db);
    ratio.reset(processSpec.sampleRate, smoothing_time);
    ratio.setCurrentAndTargetValue(raw_ratio);
}

void Compressor::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
    reset();
}

void Compressor::computeGainReductionOptometric(float& sample, float sampleRate)
{
    // Optometric Compressor
    //
    // Envelope processing
    float input_level = std::abs(sample);
    float input_level_db = juce::Decibels::gainToDecibels(input_level + 1e-10f);
    float current_threshold_db = threshold_db.getNextValue();
    float current_ratio = ratio.getNextValue();

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

    float drive = gain_smooth_db * optoParams.gr_to_sat;
    float k = (sample >= 0.0f) ? 0.8f : 1.2f;
    float sat_sample = std::tanh(k * drive * sample);
    sample = ((1 - optoParams.saturation) * sample +
              optoParams.saturation * sat_sample) *
             gain_smooth;
}

void Compressor::computeGainReductionFet(float& sample, float sampleRate)
{
    float output = sample;
    float output_level =
        std::max(std::abs(gain_smooth * output), current_level * 0.99f);
    float output_level_db =
        juce::Decibels::gainToDecibels(output_level + 1e-10f);
    float current_threshold_db = threshold_db.getNextValue();
    float current_ratio = static_cast<float>(ratio.getNextValue());

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

    float drive = gain_smooth_db * fetParams.gr_to_sat;
    float k = (sample >= 0.0f) ? 0.8f : 1.2f;
    float s = (sample >= 0.0f) ? 1.0f : -1.0f;
    float t = s * std::tanh(k * drive * sample);
    float sat_sample = s * (1.0f - (1.0f - t) * (1.0f - t));
    sample = ((1 - fetParams.saturation) * sample +
              fetParams.saturation * sat_sample) *
             gain_smooth;
    current_level = sample;
    current_level_db =
        juce::Decibels::gainToDecibels(std::abs(current_level) + 1e-10f);
}

void Compressor::computeGainReductionVca(float& sample, float sampleRate)
{
    float input_level = std::abs(sample);
    float current_threshold_db = threshold_db.getNextValue();
    float current_ratio = ratio.getNextValue();

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
        coef = std::exp(std::log(0.01f) / (sampleRate * vcaParams.attack));
    }
    else
    {
        coef = std::exp(std::log(0.01f) / (sampleRate * vcaParams.release));
    }
    current_level_db =
        (coef * current_level_db) + ((1.0f - coef) * rms_level_db);

    float target_gain_db;
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

    gain_smooth_db = target_gain_db;
    gain_smooth = juce::Decibels::decibelsToGain(gain_smooth_db);

    sample = sample * gain_smooth;
}

void Compressor::process(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    float sampleRate = static_cast<float>(processSpec.sampleRate);
    auto& block = context.getOutputBlock();
    const size_t num_samples = block.getNumSamples();

    if (type != lastType)
    {
        switch (type)
        {
        case 0:
            gainFunction = &Compressor::computeGainReductionOptometric;
            break;
        case 1:
            gainFunction = &Compressor::computeGainReductionFet;
            break;
        case 2:
            gainFunction = &Compressor::computeGainReductionVca;
            break;
        }
        lastType = type;
    }

    auto* ch = block.getChannelPointer(0);
    for (int i = 0; i < (int)num_samples; ++i)
    {
        float wet = ch[i];
        float dry = ch[i];

        (this->*gainFunction)(wet, sampleRate);

        float current_lvl = level.getNextValue();
        float current_mix = mix.getNextValue();

        ch[i] = (dry * (1.0f - current_mix) + wet * current_mix * current_lvl);
    }
}

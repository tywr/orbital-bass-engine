#include "compressor.h"

#include <algorithm>
#include <cmath>
#include <juce_dsp/juce_dsp.h>

void Compressor::reset()
{
    current_level = 1.0f;
    current_level_db = 0.0f;
    gr = 1.0f;
    gr_db = 0.0f;

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
        coef = std::exp(-1.0f / (sampleRate * optoParams.attack));
    }
    else
    {
        if (current_level_db > current_threshold_db)
        {
            coef = std::exp(-1.0f / (sampleRate * optoParams.release1));
        }
        else
        {
            coef = std::exp(-1.0f / (sampleRate * optoParams.release2));
        }
    }
    current_level = (coef * current_level) + ((1.0f - coef) * input_level);
    current_level_db = juce::Decibels::gainToDecibels(current_level);

    // Gain computer
    float a = optoParams.w / std::log(1.0f + optoParams.mu);
    float db =
        a * std::log(
                1.0f +
                optoParams.mu *
                    std::exp(
                        (current_level_db - current_threshold_db) / optoParams.w
                    )
            );
    float kr = 1.0f - 1.0f / current_ratio;
    float output_level_db = current_level_db - kr * db;

    gr_db = output_level_db - current_level_db;
    gr = juce::Decibels::decibelsToGain(gr_db);
    sample = juce::Decibels::decibelsToGain(output_level_db);
}

void Compressor::computeGainReductionFet(float& sample, float sampleRate)
{
    float input_level = std::abs(sample);
    float input_level_db = juce::Decibels::gainToDecibels(input_level + 1e-10f);
    float current_threshold_db = threshold_db.getNextValue();
    float current_ratio = static_cast<float>(ratio.getNextValue());

    float coef;
    if (input_level_db < current_level_db)
    {
        coef = std::exp(-1.0f / (sampleRate * fetParams.attack));
    }
    else
    {
        coef = std::exp(-1.0f / (sampleRate * fetParams.release));
    }
    current_level = (coef * current_level) + ((1.0f - coef) * input_level);
    current_level_db = juce::Decibels::gainToDecibels(current_level);

    float output_level_db;
    if (current_level_db <= current_threshold_db - fetParams.w)
    {
        output_level_db = current_level_db;
    }
    else if (current_level_db <= current_threshold_db + fetParams.w)
    {
        float ot = (current_level_db - current_threshold_db + fetParams.w);
        output_level_db = current_level_db + (1.0f / current_ratio - 1.0f) *
                                                 (ot * ot) /
                                                 (4.0f * fetParams.w);
    }
    else
    {
        output_level_db =
            current_threshold_db +
            (current_level_db - current_threshold_db) / current_ratio;
    }

    gr_db = output_level_db - current_level_db;
    gr = juce::Decibels::decibelsToGain(gr_db);
    sample = juce::Decibels::decibelsToGain(output_level_db);
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

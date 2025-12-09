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

    hpf_freq.reset(processSpec.sampleRate, smoothing_time);
    hpf_freq.setCurrentAndTargetValue(raw_hpf_freq);
    mix.reset(processSpec.sampleRate, smoothing_time);
    mix.setCurrentAndTargetValue(raw_mix);
    level.reset(processSpec.sampleRate, smoothing_time);
    level.setCurrentAndTargetValue(raw_level);
    threshold_db.reset(processSpec.sampleRate, smoothing_time);
    threshold_db.setCurrentAndTargetValue(raw_threshold_db);
    ratio.reset(processSpec.sampleRate, smoothing_time);
    ratio.setCurrentAndTargetValue(raw_ratio);

    updateHPF();
}

void Compressor::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
    hpf_filter.prepare(spec);
    reset();
}

void Compressor::updateHPF()
{
    float current_hpf_freq = hpf_freq.getCurrentValue();
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        processSpec.sampleRate, current_hpf_freq
    );
    *hpf_filter.coefficients = *coeffs;
}

void Compressor::computeGainReductionFet(float& sample, float sampleRate)
{
    // CALCULATE GAIN (Using previous envelope state)
    // We determine the gain for THIS sample based on the envelope from the LAST
    // sample.

    // Convert envelope voltage to dB for the threshold logic
    // (Optimization: You can do this whole section in linear voltage to save
    // CPU, but we will keep your threshold/knee logic for now).
    float env_db = juce::Decibels::gainToDecibels(envelope_state + 1e-10f);
    float current_threshold_db = threshold_db.getNextValue();
    float current_ratio = static_cast<float>(ratio.getNextValue());

    float output_level_db = env_db;

    if (env_db > current_threshold_db - width)
    {
        if (env_db <= current_threshold_db + width)
        {
            // Soft Knee
            float ot = (env_db - current_threshold_db + width);
            output_level_db = env_db + (1.0f / current_ratio - 1.0f) *
                                           (ot * ot) / (4.0f * width);
        }
        else
        {
            // Above Knee (Ratio)
            output_level_db = current_threshold_db +
                              (env_db - current_threshold_db) / current_ratio;
        }
    }

    // Calculate how much we need to reduce
    gr_db = output_level_db - env_db;
    gr = juce::Decibels::decibelsToGain(gr_db);

    // Apply the calculated gain to the input sample
    float output_sample = sample * gr;

    // Overwrite the input sample (for the host)
    sample = output_sample;

    // Sidechain High Pass (Optional: standard 1176 doesn't have this, but
    // modern plugins do)
    float detector_input = hpf_filter.processSample(output_sample);
    float rectified_input = std::abs(detector_input);

    // Ballistics (Attack/Release)
    float current_attack = 0.001f * attack.getNextValue();
    float current_release = 0.001f * release.getNextValue();

    float coef;
    if (rectified_input > envelope_state)
    {
        coef = std::exp(-1.0f / (sampleRate * current_attack));
    }
    else
    {
        coef = std::exp(-1.0f / (sampleRate * current_release));
    }

    // Update the "Capacitor" (One-pole filter)
    envelope_state =
        (coef * envelope_state) + ((1.0f - coef) * rectified_input);

    // Store GR for metering (optional)
}

void Compressor::process(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    float sampleRate = static_cast<float>(processSpec.sampleRate);
    auto& block = context.getOutputBlock();
    const size_t num_samples = block.getNumSamples();

    if (hpf_freq.isSmoothing())
    {
        hpf_freq.skip((int)num_samples);
        updateHPF();
    }

    auto* ch = block.getChannelPointer(0);
    for (int i = 0; i < (int)num_samples; ++i)
    {
        float wet = ch[i];
        float dry = ch[i];

        computeGainReductionFet(wet, sampleRate);

        float current_lvl = level.getNextValue();
        float current_mix = mix.getNextValue();

        ch[i] = (dry * (1.0f - current_mix) + wet * current_mix * current_lvl);
    }
}

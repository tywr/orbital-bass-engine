#include "borealis.h"
#include <algorithm>

#include <juce_dsp/juce_dsp.h>

void BorealisOverdrive::reset()
{
    diode.reset();
    oversampler2x.reset();
    resetFilters();
    resetSmoothedValues();
    prepareFilters();
}

void BorealisOverdrive::resetFilters()
{
    pre_hpf.reset();
    pre_lpf.reset();
    lowmids_lpf.reset();
    post_lpf.reset();
    post_lpf2.reset();
    post_lpf3.reset();
    x_hpf.reset();
    bass_lpf.reset();
}

void BorealisOverdrive::resetSmoothedValues()
{
    level.reset(process_spec.sampleRate, smoothing_time);
    level.setCurrentAndTargetValue(raw_level);
    drive.reset(process_spec.sampleRate, smoothing_time);
    drive.setCurrentAndTargetValue(raw_drive);
    mix.reset(process_spec.sampleRate, smoothing_time);
    mix.setCurrentAndTargetValue(raw_mix);
    cross_frequency.reset(process_spec.sampleRate, smoothing_time);
    cross_frequency.setCurrentAndTargetValue(raw_cross_frequency);
    bass_frequency.reset(process_spec.sampleRate, smoothing_time);
    bass_frequency.setCurrentAndTargetValue(raw_bass_frequency);
}

void BorealisOverdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= 2.0;
    process_spec = oversampled_spec;

    oversampler2x.reset();
    oversampler2x.initProcessing(static_cast<size_t>(spec.maximumBlockSize));

    resetSmoothedValues();

    prepareFilters();

    x_hpf.prepare(process_spec);
    updateXFilter();

    bass_lpf.prepare(process_spec);
    updateLowFilter();

    diode = GermaniumDiode(oversampled_spec.sampleRate);
}

void BorealisOverdrive::prepareFilters()
{
    pre_hpf.prepare(process_spec);
    auto pre_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            process_spec.sampleRate, pre_hpf_cutoff
        );
    *pre_hpf.coefficients = *pre_hpf_coefficients;

    pre_lpf.prepare(process_spec);
    auto pre_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, pre_lpf_cutoff
        );
    *pre_lpf.coefficients = *pre_lpf_coefficients;

    lowmids_lpf.prepare(process_spec);
    auto lowmids_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, lowmids_lpf_cutoff
        );
    *lowmids_lpf.coefficients = *lowmids_lpf_coefficients;

    post_lpf.prepare(process_spec);
    auto post_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, post_lpf_cutoff
        );
    *post_lpf.coefficients = *post_lpf_coefficients;

    post_lpf2.prepare(process_spec);
    auto post_lpf2_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, post_lpf2_cutoff
        );
    *post_lpf2.coefficients = *post_lpf2_coefficients;

    post_lpf3.prepare(process_spec);
    auto post_lpf3_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, post_lpf3_cutoff
        );
    *post_lpf3.coefficients = *post_lpf3_coefficients;
}

void BorealisOverdrive::updateXFilter()
{
    float current_x_frequency = std::max(cross_frequency.getNextValue(), 1.0f);
    auto x_coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        process_spec.sampleRate, current_x_frequency
    );
    *x_hpf.coefficients = *x_coefficients;
}

void BorealisOverdrive::updateLowFilter()
{
    float current_bass_frequency =
        std::max(bass_frequency.getNextValue(), 1.0f);
    auto bass_coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        process_spec.sampleRate, current_bass_frequency
    );
    *bass_lpf.coefficients = *bass_coefficients;
}

float BorealisOverdrive::driveToGain(float d)
{
    float t = d / 10.0f;
    float min_gain_db = 20.0f;
    float max_gain_db = 64.0f;
    float gain = juce::Decibels::decibelsToGain(
        min_gain_db + t * (max_gain_db - min_gain_db)
    );
    return gain;
}

void BorealisOverdrive::process(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    auto& block = context.getOutputBlock();

    if (bypass)
    {
        return;
    }

    auto oversampled_block = oversampler2x.processSamplesUp(block);

    auto* ch = oversampled_block.getChannelPointer(0);
    for (size_t i = 0; i < oversampled_block.getNumSamples(); ++i)
    {
        if (cross_frequency.isSmoothing())
            updateXFilter();

        if (bass_frequency.isSmoothing())
            updateLowFilter();

        float dry = ch[i];
        float wet = ch[i];
        applyOverdrive(wet);

        float current_mix = mix.getNextValue();
        ch[i] = wet * current_mix + dry * (1.0f - current_mix);
    }
    oversampler2x.processSamplesDown(block);
}

void BorealisOverdrive::applyOverdrive(float& sample)
{
    float current_drive = drive.getNextValue();
    float current_level = level.getNextValue();
    float current_drive_gain = driveToGain(current_drive);

    // Clean the input signal with LPF and HPF
    float in = pre_lpf.processSample(pre_hpf.processSample(sample));

    // Get the fixed lowmids signal
    float bass = bass_lpf.processSample(in);

    // Get the X-over Signal
    float x_signal = current_drive_gain * x_hpf.processSample(in);
    float distorted = x_output_padding * diode.processSample(x_signal);

    float lpf1 = post_lpf.processSample(distorted);
    float lpf2 = post_lpf2.processSample(lpf1);
    float x_out = post_lpf3.processSample(lpf2);

    // Mix the lowmids and the distorted X-over signal
    sample = bass + current_level * x_out;
}

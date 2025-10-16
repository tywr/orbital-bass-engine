#include "borealis.h"
#include <algorithm>

#include <juce_dsp/juce_dsp.h>

void BorealisOverdrive::resetSmoothedValues()
{
    level.reset(processSpec.sampleRate, smoothing_time);
    level.setCurrentAndTargetValue(raw_level);
    drive.reset(processSpec.sampleRate, smoothing_time);
    drive.setCurrentAndTargetValue(raw_drive);
    mix.reset(processSpec.sampleRate, smoothing_time);
    mix.setCurrentAndTargetValue(raw_mix);
    cross_frequency.reset(processSpec.sampleRate, smoothing_time);
    cross_frequency.setCurrentAndTargetValue(raw_cross_frequency);
    high_level.reset(processSpec.sampleRate, smoothing_time);
    high_level.setCurrentAndTargetValue(raw_high_level);
}

void BorealisOverdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= 2.0;
    processSpec = oversampled_spec;

    oversampler2x.reset();
    oversampler2x.initProcessing(static_cast<size_t>(spec.maximumBlockSize));

    resetSmoothedValues();

    prepareFilters();

    x_hpf.prepare(processSpec);
    updateXFilter();

    diode = GermaniumDiode(oversampled_spec.sampleRate);
}

void BorealisOverdrive::prepareFilters()
{
    pre_hpf.prepare(processSpec);
    auto pre_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            processSpec.sampleRate, pre_hpf_cutoff
        );
    *pre_hpf.coefficients = *pre_hpf_coefficients;

    pre_lpf.prepare(processSpec);
    auto pre_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, pre_lpf_cutoff
        );
    *pre_lpf.coefficients = *pre_lpf_coefficients;

    lowmids_lpf.prepare(processSpec);
    auto lowmids_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, lowmids_lpf_cutoff
        );
    *lowmids_lpf.coefficients = *lowmids_lpf_coefficients;

    post_lpf.prepare(processSpec);
    auto post_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, post_lpf_cutoff
        );
    *post_lpf.coefficients = *post_lpf_coefficients;

    post_lpf2.prepare(processSpec);
    auto post_lpf2_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, post_lpf2_cutoff
        );
    *post_lpf2.coefficients = *post_lpf2_coefficients;

    post_lpf3.prepare(processSpec);
    auto post_lpf3_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, post_lpf3_cutoff
        );
    *post_lpf3.coefficients = *post_lpf3_coefficients;
}

void BorealisOverdrive::updateXFilter()
{
    float current_x_frequency =
        std::max(cross_frequency.getNextValue(), 1.0f);
    auto x_coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        processSpec.sampleRate, current_x_frequency
    );
    *x_hpf.coefficients = *x_coefficients;
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

void BorealisOverdrive::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    juce::dsp::AudioBlock<float> block(buffer);
    auto oversampledBlock = oversampler2x.processSamplesUp(block);

    auto* channelData = oversampledBlock.getChannelPointer(0);
    for (size_t i = 0; i < oversampledBlock.getNumSamples(); ++i)
    {
        if (cross_frequency.isSmoothing())
            updateXFilter();

        float dry = channelData[i];
        float wet = channelData[i];
        applyOverdrive(wet);

        float current_level = level.getNextValue();
        float current_mix = mix.getNextValue();
        wet *= current_level;
        channelData[i] = wet * current_mix + dry * (1.0f - current_mix);
    }
    oversampler2x.processSamplesDown(block);
}

void BorealisOverdrive::applyOverdrive(float& sample)
{
    float current_drive = drive.getNextValue();
    float current_high_level = high_level.getNextValue();
    float current_drive_gain = driveToGain(current_drive);

    // Clean the input signal with LPF and HPF
    float in = pre_lpf.processSample(pre_hpf.processSample(sample));

    // Get the fixed lowmids signal
    float lowmids = lowmids_lpf.processSample(in);

    // Get the X-over Signal
    float x_signal = current_drive_gain * x_hpf.processSample(in);
    float distorted = x_output_padding * diode.processSample(x_signal);

    float lpf1 = post_lpf.processSample(distorted);
    float lpf2 = post_lpf2.processSample(lpf1);
    float x_out = post_lpf3.processSample(lpf2);

    // Mix the lowmids and the distorted X-over signal
    sample = lowmids + current_high_level * x_out;
}

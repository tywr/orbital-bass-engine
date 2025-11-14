#include "borealis.h"
#include "../filters/drive_filter.h"
#include <algorithm>

#include <juce_dsp/juce_dsp.h>

void BorealisOverdrive::reset()
{
    cmos.reset();
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
    x_hpf.reset();
    bass_lpf.reset();
    drive_filter.reset();
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

    high_buffer.setSize(
        (int)process_spec.numChannels, (int)process_spec.maximumBlockSize * 4,
        false, false, true
    );
    low_buffer.setSize(
        (int)process_spec.numChannels, (int)process_spec.maximumBlockSize * 4,
        false, false, true
    );

    cmos.prepare();
    resetSmoothedValues();
    prepareFilters();
}

void BorealisOverdrive::prepareFilters()
{
    x_hpf.prepare(process_spec);
    updateXFilter();

    bass_lpf.prepare(process_spec);
    updateLowFilter();

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

void BorealisOverdrive::updateDriveFilter()
{
    float current_drive = drive.getCurrentValue();

    // Set the frequency based on the grunt parameter
    float rolloff_frequency = 3200.0f;
    float drive_frequency = 209.0f;

    float min_gain_db = 0.0f;
    float max_gain_db = 40.0f;
    float drive_filter_gain = juce::Decibels::decibelsToGain(
        min_gain_db + (max_gain_db - min_gain_db) * current_drive * 0.1f
    );

    auto drive_filter_coefficients = makeDriveFilter(
        (float)process_spec.sampleRate, drive_frequency, rolloff_frequency,
        drive_filter_gain
    );
    *drive_filter.coefficients = *drive_filter_coefficients;
}

void BorealisOverdrive::process(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{

    auto& block = context.getOutputBlock();
    auto oversampled_block = oversampler2x.processSamplesUp(block);

    const size_t num_samples = oversampled_block.getNumSamples();

    if (cross_frequency.isSmoothing())
    {
        cross_frequency.skip((int)num_samples);
        updateXFilter();
    }
    if (bass_frequency.isSmoothing())
    {
        bass_frequency.skip((int)num_samples);
        updateLowFilter();
    }
    if (drive.isSmoothing())
    {
        drive.skip((int)num_samples);
        updateDriveFilter();
    }

    juce::dsp::AudioBlock<float> high_block(high_buffer);
    juce::dsp::AudioBlock<float> low_block(low_buffer);
    auto high_sub = high_block.getSubBlock(0, num_samples);
    auto low_sub = low_block.getSubBlock(0, num_samples);
    high_sub.copyFrom(oversampled_block);
    low_sub.copyFrom(oversampled_block);

    // Process the low only
    auto low_context = juce::dsp::ProcessContextReplacing<float>(low_sub);
    pre_hpf.process(low_context);
    bass_lpf.process(low_context);

    // Process the highs only
    auto high_context = juce::dsp::ProcessContextReplacing<float>(high_sub);
    pre_lpf.process(high_context);
    x_hpf.process(high_context);
    drive_filter.process(high_context);
    cmos.process(high_context);
    post_lpf.process(high_context);
    post_lpf2.process(high_context);

    auto* ch = oversampled_block.getChannelPointer(0);
    auto* low = low_sub.getChannelPointer(0);
    auto* high = high_sub.getChannelPointer(0);
    for (size_t i = 0; i < num_samples; ++i)
    {
        float current_mix = mix.getNextValue();
        float current_level = level.getNextValue();

        float dry = ch[i];
        float od = low[i] + current_level * high[i];
        ch[i] = od * current_mix + dry * (1.0f - current_mix);
    }
    oversampler2x.processSamplesDown(block);
}

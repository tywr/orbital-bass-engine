#include "nebula.h"
#include "../circuits/cmos.h"
#include "../circuits/opamp.h"
#include "../filters/drive_filter.h"

#include <juce_dsp/juce_dsp.h>

void NebulaOverdrive::reset()
{
    cmos.reset();
}

void NebulaOverdrive::resetSmoothedValues()
{
    float sample_rate = static_cast<float>(processSpec.sampleRate);
    sample_rate = std::max(1.0f, sample_rate);

    level.reset(sample_rate, smoothing_time);
    level.setCurrentAndTargetValue(raw_level);
    drive.reset(sample_rate, smoothing_time);
    drive.setCurrentAndTargetValue(raw_drive);
    mix.reset(sample_rate, smoothing_time);
    mix.setCurrentAndTargetValue(raw_mix);
    mod.reset(sample_rate, smoothing_time);
    mod.setCurrentAndTargetValue(raw_mod);
    aggro.reset(sample_rate, smoothing_time);
    aggro.setCurrentAndTargetValue(raw_aggro);
}

void NebulaOverdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= 2.0;
    processSpec = oversampled_spec;

    oversampler2x.reset();
    oversampler2x.initProcessing(static_cast<size_t>(spec.maximumBlockSize));

    resetSmoothedValues();

    prepareFilters();
}

void NebulaOverdrive::prepareFilters()
{
    drive_filter.prepare(processSpec);
    updateDriveFilter();

    pre_hpf.prepare(processSpec);
    auto pre_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            processSpec.sampleRate, pre_hpf_cutoff
        );
    *pre_hpf.coefficients = *pre_hpf_coefficients;

    alpha_1_filter.prepare(processSpec);
    auto alpha_1_coefficients =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            processSpec.sampleRate, alpha_1_frequency, 0.707f, alpha_1_gain
        );
    *alpha_1_filter.coefficients = *alpha_1_coefficients;

    alpha_2_filter.prepare(processSpec);
    auto alpha_2_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            processSpec.sampleRate, alpha_2_frequency
        );
    *alpha_2_filter.coefficients = *alpha_2_coefficients;

    alpha_3_filter.prepare(processSpec);
    auto alpha_3_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            processSpec.sampleRate, alpha_3_frequency
        );
    *alpha_3_filter.coefficients = *alpha_3_coefficients;

    omega_1_filter.prepare(processSpec);
    auto omega_1_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            processSpec.sampleRate, omega_1_frequency
        );
    *omega_1_filter.coefficients = *omega_1_coefficients;

    omega_2_filter.prepare(processSpec);
    auto omega_2_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, omega_2_frequency, 3.0f
        );
    *omega_2_filter.coefficients = *omega_2_coefficients;

    omega_3_filter.prepare(processSpec);
    auto omega_3_coefficients =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            processSpec.sampleRate, omega_3_frequency, 0.707f, omega_3_gain
        );
    *omega_3_filter.coefficients = *omega_3_coefficients;

    lpf_filter.prepare(processSpec);
    auto lpf_coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        processSpec.sampleRate, lpf_cutoff, 0.707f
    );
    *lpf_filter.coefficients = *lpf_coefficients;
}

void NebulaOverdrive::updateDriveFilter()
{
    float current_drive = drive.getCurrentValue();
    float drive_filter_q = 0.707f;

    float rolloff_frequency = 3200.0f;
    float drive_frequency = 209.0f;

    // Set the gain based on the drive parameter
    float min_gain_db = 0.0f;
    float max_gain_db = 36.0f;
    float drive_filter_gain = juce::Decibels::decibelsToGain(
        min_gain_db + (max_gain_db - min_gain_db) * current_drive * 0.1f
    );

    auto drive_filter_coefficients = makeDriveFilter(
        processSpec.sampleRate, drive_frequency, rolloff_frequency,
        drive_filter_gain
    );
    *drive_filter.coefficients = *drive_filter_coefficients;
}

void NebulaOverdrive::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    juce::AudioBuffer<float> dry_buffer;
    dry_buffer.makeCopyOf(buffer);

    juce::dsp::AudioBlock<float> block(buffer);
    auto oversampledBlock = oversampler2x.processSamplesUp(block);

    auto* channelData = oversampledBlock.getChannelPointer(0);
    for (size_t i = 0; i < oversampledBlock.getNumSamples(); ++i)
    {
        if (drive.isSmoothing())
        {
            drive.getNextValue();
            updateDriveFilter();
        }
        if (mod.isSmoothing())
        {
            mod.getNextValue();
        }

        float wet = channelData[i];
        float dry = channelData[i];
        applyOverdrive(wet);

        float current_level = level.getNextValue();
        float current_mix = mix.getNextValue();
        wet *= current_level;
        channelData[i] = wet * current_mix + dry * (1.0f - current_mix);
    }
    oversampler2x.processSamplesDown(block);
}

void NebulaOverdrive::applyOverdrive(float& sample)
{
    float current_mod = 0.1f * mod.getCurrentValue();

    float drived = drive_filter.processSample(sample);

    float alpha_1 = alpha_1_filter.processSample(drived);
    float alpha_out = cmos.processSample(alpha_1);

    float omega_1 = omega_1_filter.processSample(drived);
    float omega_2 = omega_2_filter.processSample(omega_1);
    // float omega_3 = opamp.processSample(omega_2);
    float omega_3 = cmos.processSample(omega_2);
    float omega_out = omega_3_filter.processSample(omega_3);

    float out = alpha_out * (1.0f - current_mod) + omega_out * current_mod;
    sample = lpf_filter.processSample(out);
}

#include "nebula.h"

#include <juce_dsp/juce_dsp.h>

void NebulaOverdrive::resetSmoothedValues()
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

void NebulaOverdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= 2.0;
    processSpec = oversampled_spec;

    oversampler2x.reset();
    oversampler2x.initProcessing(static_cast<size_t>(spec.maximumBlockSize));

    resetSmoothedValues();

    prepareFilters();

    diode = GermaniumDiode(oversampled_spec.sampleRate);
}

void NebulaOverdrive::prepareFilters()
{
    ff1_hpf.prepare(processSpec);
    auto ff1_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, ff1_hpf_cutoff
        );
    *ff1_hpf.coefficients = *ff1_hpf_coefficients;

    ff1_lpf.prepare(processSpec);
    auto ff1_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, ff1_lpf_cutoff
        );
    *ff1_lpf.coefficients = *ff1_lpf_coefficients;

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

    post_lpf.prepare(processSpec);
    auto post_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, post_lpf_cutoff
        );
    *post_lpf.coefficients = *post_lpf_coefficients;
}

float NebulaOverdrive::driveToFrequency(float d)
{
    float t = d / 10.0f;
    float min_frequency = 50.0f;
    float max_frequency = 450.0f;
    return max_frequency - (max_frequency - min_frequency) * std::pow(t, 2.0f);
}

float NebulaOverdrive::driveToGain(float d)
{
    float t = d / 10.0f;
    float min_gain_db = 0.0f;
    float max_gain_db = 36.0f;
    return juce::Decibels::decibelsToGain(
        min_gain_db + (max_gain_db - min_gain_db) * t
    );
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
    float input_padding = juce::Decibels::decibelsToGain(12.0f);
    float current_drive = drive.getNextValue();
    float drive_gain = driveToGain(current_drive);
    float in = input_padding * sample;
    float in_drive = drive_gain * in;

    float ff1 = ff1_hpf.processSample(ff1_lpf.processSample(in));

    float ff2 = 0.1f * current_drive * ff1;

    // distortion chain
    float hpfed = pre_hpf.processSample(in_drive);
    float lpfed = pre_lpf.processSample(hpfed);
    float shaped = attack_shelf.processSample(lpfed);
    float distorded = diode.processSample(shaped);
    float out = post_lpf.processSample(distorded);

    sample = padding * (out + ff1 + ff2);
}

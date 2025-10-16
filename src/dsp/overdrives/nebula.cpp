#include "nebula.h"

#include <juce_dsp/juce_dsp.h>

void NebulaOverdrive::reset()
{
}

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
    float current_drive = drive.getNextValue();
    float drive_gain = driveToGain(current_drive);

    sample = sample * drive_gain;
}

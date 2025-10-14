#include "nebula.h"

#include <juce_dsp/juce_dsp.h>

void NebulaOverdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= 2.0;
    processSpec = oversampled_spec;

    oversampler2x.reset();
    oversampler2x.initProcessing(static_cast<size_t>(spec.maximumBlockSize));

    attack_shelf.prepare(oversampled_spec);
    updateAttackFilter();

    ff1_hpf.prepare(oversampled_spec);
    auto ff1_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, ff1_hpf_cutoff
        );
    *ff1_hpf.coefficients = *ff1_hpf_coefficients;

    ff1_lpf.prepare(oversampled_spec);
    auto ff1_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, ff1_lpf_cutoff
        );
    *ff1_lpf.coefficients = *ff1_lpf_coefficients;

    pre_hpf.prepare(oversampled_spec);
    auto pre_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            oversampled_spec.sampleRate, pre_hpf_cutoff
        );
    *pre_hpf.coefficients = *pre_hpf_coefficients;

    pre_lpf.prepare(oversampled_spec);
    auto pre_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, pre_lpf_cutoff
        );
    *pre_lpf.coefficients = *pre_lpf_coefficients;

    post_lpf.prepare(oversampled_spec);
    auto post_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, post_lpf_cutoff
        );
    *post_lpf.coefficients = *post_lpf_coefficients;

    diode = GermaniumDiode(oversampled_spec.sampleRate);
}

float NebulaOverdrive::driveToFrequency(float d)
{
    float t = d / 10.0f;
    float min_frequency = 50.0f;
    float max_frequency = 450.0f;
    return max_frequency - (max_frequency - min_frequency) * std::pow(t, 2.0f);
}

void NebulaOverdrive::updateAttackFilter()
{
    float min_gain_db = -12.0f;
    float max_gain_db = 12.0f;
    float shelf_gain = juce::Decibels::decibelsToGain(
        min_gain_db + (max_gain_db - min_gain_db) * current_attack / 10.0f
    );
    auto attack_shelf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            processSpec.sampleRate, attack_shelf_freq, 0.7f, shelf_gain
        );
    *attack_shelf.coefficients = *attack_shelf_coefficients;
}

void NebulaOverdrive::setCoefficients()
{
    if (std::abs(current_attack - attack) >= 1e-2)
    {
        current_attack += (attack - current_attack) * 0.1f;
        updateAttackFilter();
    }
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

    float sampleRate = static_cast<float>(processSpec.sampleRate);
    setCoefficients();

    juce::dsp::AudioBlock<float> block(buffer);
    auto oversampledBlock = oversampler2x.processSamplesUp(block);

    auto* channelData = oversampledBlock.getChannelPointer(0);
    for (size_t i = 0; i < oversampledBlock.getNumSamples(); ++i)
    {
        float sample = channelData[i];
        applyOverdrive(sample, sampleRate);
        channelData[i] = sample;
    }
    oversampler2x.processSamplesDown(block);

    applyGain(buffer, previous_level, level);
    buffer.applyGain(mix);
    dry_buffer.applyGain(1.0f - mix);
    buffer.addFrom(0, 0, dry_buffer, 0, 0, buffer.getNumSamples());
}

void NebulaOverdrive::applyOverdrive(float& sample, float sampleRate)
{
    juce::ignoreUnused(sampleRate);
    float input_padding = juce::Decibels::decibelsToGain(12.0f);

    float drive_gain = driveToGain(drive);
    float in = input_padding * sample;
    float in_drive = drive_gain * in;

    // feed forward 1
    float ff1 = ff1_hpf.processSample(ff1_lpf.processSample(in));

    float ff2 = 0.1f * drive * ff1;

    // distortion chain
    float hpfed = pre_hpf.processSample(in_drive);
    float lpfed = pre_lpf.processSample(hpfed);
    float shaped = attack_shelf.processSample(lpfed);
    float distorded = diode.processSample(shaped);
    float out = post_lpf.processSample(distorded);

    sample = padding * (out + ff1 + ff2);
}

#include "Helios.h"
#include "../circuits/silicon_diode.h"

#include <juce_dsp/juce_dsp.h>

void HeliosOverdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= 2;
    processSpec = oversampled_spec;

    oversampler2x.reset();
    oversampler2x.initProcessing(static_cast<size_t>(spec.maximumBlockSize));

    dc_hpf.prepare(oversampled_spec);
    auto dc_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            oversampled_spec.sampleRate, dc_hpf_cutoff
        );
    *dc_hpf.coefficients = *dc_hpf_coefficients;

    dc_hpf2.prepare(oversampled_spec);
    auto dc_hpf2_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            oversampled_spec.sampleRate, dc_hpf2_cutoff
        );
    *dc_hpf2.coefficients = *dc_hpf2_coefficients;

    pre_lpf.prepare(oversampled_spec);
    auto pre_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, pre_lpf_cutoff, pre_lpf_q
        );
    *pre_lpf.coefficients = *pre_lpf_coefficients;

    lowmids_lpf.prepare(oversampled_spec);
    auto lowmids_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, lowmids_lpf_cutoff
        );
    *lowmids_lpf.coefficients = *lowmids_lpf_coefficients;

    mid_hpf.prepare(oversampled_spec);
    auto mid_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            oversampled_spec.sampleRate, mid_hpf_cutoff
        );
    *mid_hpf.coefficients = *mid_hpf_coefficients;

    pre_hpf.prepare(oversampled_spec);
    auto pre_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            oversampled_spec.sampleRate, pre_hpf_cutoff
        );
    *pre_hpf.coefficients = *pre_hpf_coefficients;

    era_lpf.prepare(oversampled_spec);
    auto era_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, era_lpf_cutoff, era_lpf_q
        );
    *era_lpf.coefficients = *era_lpf_coefficients;

    post_lpf.prepare(oversampled_spec);
    auto post_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, post_lpf_cutoff, post_lpf_q
        );
    *post_lpf.coefficients = *post_lpf_coefficients;

    post_lpf2.prepare(oversampled_spec);
    auto post_lpf2_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, post_lpf2_cutoff
        );
    *post_lpf2.coefficients = *post_lpf2_coefficients;

    diode_plus = SiliconDiode(oversampled_spec.sampleRate, true);
    diode_minus = SiliconDiode(oversampled_spec.sampleRate, false);
}

float HeliosOverdrive::driveToGain(float d)
{
    float t = d / 10.0f;
    // version 1
    // float min_gain = juce::Decibels::decibelsToGain(3.0f);
    // float max_gain = juce::Decibels::decibelsToGain(18.0f);
    // version 2
    float min_gain = juce::Decibels::decibelsToGain(0.0f);
    float max_gain = juce::Decibels::decibelsToGain(36.0f);
    return min_gain + std::pow(t, 2.0f) * (max_gain - min_gain);
}

float HeliosOverdrive::charToFreq(float c)
{
    float t = character / 10.0f;
    float max_value = 10000.0f;
    float min_value = 1600.0f;
    return min_value + std::pow(t, 2) * (max_value - min_value);
}

void HeliosOverdrive::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    juce::AudioBuffer<float> dry_buffer;
    dry_buffer.makeCopyOf(buffer);

    // applyGain(buffer, previous_drive_gain, drive_gain);
    float sampleRate = static_cast<float>(processSpec.sampleRate);
    // Update era cutoff
    float new_era_lpf_cutoff = charToFreq(character);

    if (!juce::approximatelyEqual(era_lpf_cutoff, new_era_lpf_cutoff))
    {
        era_lpf_cutoff = new_era_lpf_cutoff;
        auto era_lpf_coefficients =
            juce::dsp::IIR::Coefficients<float>::makeLowPass(
                sampleRate, era_lpf_cutoff
            );
        *era_lpf.coefficients = *era_lpf_coefficients;
    }

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
};

void HeliosOverdrive::applyOverdrive(float& sample, float sampleRate)
{
    juce::ignoreUnused(sampleRate);

    float drive_gain = driveToGain(drive);

    float input_padding = juce::Decibels::decibelsToGain(12.0f);
    float filtered =
        pre_lpf.processSample(pre_hpf.processSample(input_padding * sample));
    // Only drive low-mids and up
    float drived = mid_hpf.processSample(drive_gain * filtered);
    // Filter low mids on "clean" signal
    float lowmids = lowmids_lpf.processSample(filtered);
    // Combine both driven and clean signal before overdrive
    // Add a bit more clean signal to keep lowend
    float input = drived + lowmids;
    // Pass signal through silicon diode circuit to limit signal above -0.7
    // Below diode cutting point of 0.7V, we only get CMOS inverter
    // saturation.
    float soft_clipped = .5 * (diode_plus.processSample(input) + input);
    float hard_clipped = diode_minus.processSample(soft_clipped);
    // Pass signal through CMOS inverter on linear region
    float distorted =
        dc_hpf.processSample(cmos.processSample(4.5f + hard_clipped));
    // Apply tone control "era"
    float era = .5 * (era_lpf.processSample(distorted) + distorted);
    // Apply first post filter (remove hiss and high frequency noise)
    float lpfed = post_lpf.processSample(era);
    float out = post_lpf2.processSample(lpfed);
    sample = padding * out;
}

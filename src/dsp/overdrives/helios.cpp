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

    // set era and update filters that depend on it
    era_mid_scoop.prepare(oversampled_spec);
    era_shelf.prepare(oversampled_spec);
    updateEraFilter(oversampled_spec.sampleRate);

    dc_hpf.prepare(oversampled_spec);
    auto dc_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            oversampled_spec.sampleRate, dc_hpf_cutoff
        );
    *dc_hpf.coefficients = *dc_hpf_coefficients;

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

    post_lpf3.prepare(oversampled_spec);
    auto post_lpf3_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            oversampled_spec.sampleRate, post_lpf3_cutoff
        );
    *post_lpf3.coefficients = *post_lpf3_coefficients;

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
    return min_gain + std::pow(t, 3.0f) * (max_gain - min_gain);
}

void HeliosOverdrive::updateEraFilter(float sampleRate)
{
    // A bit of a complex filter happening there, the era control
    // is a combination of a mid scoop and a high shelf filters.
    // When the era knob is turned, it shifts the mid scoop frequency
    // from ~1200Hz down to 700Hz and reduces the width of the scoop as well
    // as the gain.
    // The high shelf is fixed at ~500Hz, and adjust the amount of
    // hi-mids and treble cut.
    era = (character / 10.0f);
    era_mid_scoop_frequency = 1200.0f - 500.0f * era;
    era_mid_scoop_gain = juce::Decibels::decibelsToGain(-4.0 - 4.0f * era);
    era_mid_scoop_q = 0.5f + 0.5f * era;

    auto era_mid_scoop_coefficients =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate, era_mid_scoop_frequency, era_mid_scoop_q,
            era_mid_scoop_gain
        );
    *era_mid_scoop.coefficients = *era_mid_scoop_coefficients;

    era_shelf_cutoff = 500.0f;
    era_shelf_gain = juce::Decibels::decibelsToGain(-18.0f + 10.0f * era);
    era_shelf_q = 0.7f;

    auto era_shelf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            sampleRate, era_shelf_cutoff, era_shelf_q, era_shelf_gain
        );
    *era_shelf.coefficients = *era_shelf_coefficients;
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
    if (!juce::approximatelyEqual(character / 10.0f, era))
    {
        era = era + (character - era) * 0.1f;
        updateEraFilter(sampleRate);
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
    // Only drive between low-mids and highs
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
    float era_scoop = era_mid_scoop.processSample(distorted);
    float era = era_shelf.processSample(era_scoop);
    // Apply three consecutive LPF to smooth out top-end
    float lpfed1 = post_lpf.processSample(era);
    float lpfed2 = post_lpf2.processSample(lpfed1);
    float out = post_lpf3.processSample(lpfed2);
    sample = padding * out;
}

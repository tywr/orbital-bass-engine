#include "Helios.h"
#include "../circuits/silicon_diode.h"
#include <cmath>

#include <juce_dsp/juce_dsp.h>

void HeliosOverdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= 2;
    processSpec = oversampled_spec;

    oversampler2x.reset();
    oversampler2x.initProcessing(static_cast<size_t>(spec.maximumBlockSize));

    current_drive = drive;
    current_attack = attack;
    current_grunt = grunt;

    era_mid_scoop.prepare(oversampled_spec);
    auto era_mid_scoop_coefficients =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            oversampled_spec.sampleRate, era_mid_scoop_cutoff,
            era_mid_scoop_gain, era_mid_scoop_q
        );
    *era_mid_scoop.coefficients = *era_mid_scoop_coefficients;

    attack_shelf.prepare(oversampled_spec);
    updateAttackFilter(oversampled_spec.sampleRate);

    grunt_shelf.prepare(oversampled_spec);
    updateGruntFilter(oversampled_spec.sampleRate);

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
            oversampled_spec.sampleRate, post_lpf_cutoff
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
    float min_gain_db = 0.0f;
    float max_gain_db = 42.0f;
    return juce::Decibels::decibelsToGain(
        min_gain_db + t * (max_gain_db - min_gain_db)
    );
}

void HeliosOverdrive::updateAttackFilter(float sampleRate)
{
    float min_gain_db = -12.0f;
    float max_gain_db = 12.0f;
    float shelf_gain = juce::Decibels::decibelsToGain(
        min_gain_db + (max_gain_db - min_gain_db) * attack * 0.1f
    );
    auto attack_shelf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            sampleRate, attack_shelf_cutoff, attack_shelf_q, shelf_gain
        );
    *attack_shelf.coefficients = *attack_shelf_coefficients;
}

void HeliosOverdrive::updateGruntFilter(float sampleRate)
{
    float min_gain_db = -6.0f;
    float max_gain_db = 6.0f;
    float shelf_gain = juce::Decibels::decibelsToGain(
        min_gain_db + (max_gain_db - min_gain_db) * grunt * 0.1f
    );
    auto grunt_shelf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            sampleRate, grunt_shelf_cutoff, grunt_shelf_q, shelf_gain
        );
    *grunt_shelf.coefficients = *grunt_shelf_coefficients;
}

void HeliosOverdrive::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    juce::AudioBuffer<float> dry_buffer;
    dry_buffer.makeCopyOf(buffer);

    if (!juce::approximatelyEqual(drive, current_drive))
    {
        current_drive = current_drive + (drive - current_drive) * 0.1f;
    }

    float sampleRate = static_cast<float>(processSpec.sampleRate);
    if (!juce::approximatelyEqual(attack, current_attack))
    {
        current_attack = current_attack + (attack - current_attack) * 0.1f;
        updateAttackFilter(sampleRate);
    }
    if (!juce::approximatelyEqual(grunt, current_grunt))
    {
        current_grunt = current_grunt + (grunt - current_grunt) * 0.1f;
        updateAttackFilter(sampleRate);
    }

    juce::dsp::AudioBlock<float> block(buffer);
    auto oversampledBlock = oversampler2x.processSamplesUp(block);

    auto* channelData = oversampledBlock.getChannelPointer(0);
    for (size_t i = 0; i < oversampledBlock.getNumSamples(); ++i)
    {
        float sample = channelData[i];
        applyOverdrive(sample, sampleRate, driveToGain(current_drive));
        channelData[i] = sample;
    }
    oversampler2x.processSamplesDown(block);

    applyGain(buffer, previous_level, level);
    buffer.applyGain(mix);
    dry_buffer.applyGain(1.0f - mix);
    buffer.addFrom(0, 0, dry_buffer, 0, 0, buffer.getNumSamples());
}

void HeliosOverdrive::applyOverdrive(
    float& sample, float sampleRate, float gain
)
{
    juce::ignoreUnused(sampleRate);

    float input_padding = juce::Decibels::decibelsToGain(12.0f);
    float filtered =
        pre_lpf.processSample(pre_hpf.processSample(input_padding * sample));
    // Only drive between low-mids and highs
    float mids = mid_hpf.processSample(filtered);
    float grunted = grunt_shelf.processSample(mids);
    float drived = mid_hpf.processSample(gain * grunted);
    // Filter low mids on "clean" signal
    float lowmids = lowmids_lpf.processSample(filtered);
    // Combine both driven and clean signal before overdrive
    // Add a bit more clean signal to keep lowend
    float input = drived + lowmids;
    // Pass signal through silicon diode circuit to limit signal above -0.7
    // Below diode cutting point of 0.7V, we only get CMOS inverter
    // saturation.
    float soft_clipped = 0.5f * (diode_plus.processSample(input) + input);
    float hard_clipped = diode_minus.processSample(soft_clipped);
    // Pass signal through CMOS inverter on linear region
    float distorted =
        dc_hpf.processSample(cmos.processSample(4.5f + hard_clipped));
    float shelved = attack_shelf.processSample(distorted);
    float era = era_mid_scoop.processSample(shelved);
    // Apply three consecutive LPF to smooth out top-end
    float lpfed1 = post_lpf.processSample(era);
    float lpfed2 = post_lpf2.processSample(lpfed1);
    float out = post_lpf3.processSample(lpfed2);
    sample = padding * out;
}

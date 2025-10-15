#include "Helios.h"
#include "../circuits/silicon_diode.h"
#include <cmath>

#include <juce_dsp/juce_dsp.h>

void HeliosOverdrive::resetSmoothedValues()
{
    level.reset(processSpec.sampleRate, smoothing_time);
    level.setCurrentAndTargetValue(raw_level);
    drive.reset(processSpec.sampleRate, smoothing_time);
    drive.setCurrentAndTargetValue(raw_drive);
    mix.reset(processSpec.sampleRate, smoothing_time);
    mix.setCurrentAndTargetValue(raw_mix);
    attack.reset(processSpec.sampleRate, smoothing_time);
    attack.setCurrentAndTargetValue(raw_attack);
    grunt.reset(processSpec.sampleRate, smoothing_time);
    grunt.setCurrentAndTargetValue(raw_grunt);
}

void HeliosOverdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= 2;
    processSpec = oversampled_spec;

    oversampler2x.reset();
    oversampler2x.initProcessing(static_cast<size_t>(spec.maximumBlockSize));

    resetSmoothedValues();

    diode_plus = SiliconDiode(processSpec.sampleRate, true);
    diode_minus = SiliconDiode(processSpec.sampleRate, false);

    prepareFilters();

    attack_shelf.prepare(processSpec);
    updateAttackFilter();

    grunt_filter.prepare(processSpec);
    updateGruntFilter();
}

void HeliosOverdrive::prepareFilters()
{

    era_mid_scoop.prepare(processSpec);
    auto era_mid_scoop_coefficients =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            processSpec.sampleRate, era_mid_scoop_cutoff, era_mid_scoop_gain,
            era_mid_scoop_q
        );
    *era_mid_scoop.coefficients = *era_mid_scoop_coefficients;

    dc_hpf.prepare(processSpec);
    auto dc_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            processSpec.sampleRate, dc_hpf_cutoff
        );
    *dc_hpf.coefficients = *dc_hpf_coefficients;

    pre_lpf.prepare(processSpec);
    auto pre_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, pre_lpf_cutoff, pre_lpf_q
        );
    *pre_lpf.coefficients = *pre_lpf_coefficients;

    lowmids_lpf.prepare(processSpec);
    auto lowmids_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, lowmids_lpf_cutoff
        );
    *lowmids_lpf.coefficients = *lowmids_lpf_coefficients;

    mid_hpf.prepare(processSpec);
    auto mid_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            processSpec.sampleRate, mid_hpf_cutoff
        );
    *mid_hpf.coefficients = *mid_hpf_coefficients;

    pre_hpf.prepare(processSpec);
    auto pre_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            processSpec.sampleRate, pre_hpf_cutoff
        );
    *pre_hpf.coefficients = *pre_hpf_coefficients;

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

float HeliosOverdrive::driveToGain(float d)
{
    float t = d / 10.0f;
    float min_gain_db = 0.0f;
    float max_gain_db = 42.0f;
    float gain = juce::Decibels::decibelsToGain(
        min_gain_db + t * (max_gain_db - min_gain_db)
    );
    return gain;
}

void HeliosOverdrive::updateAttackFilter()
{
    float current_attack = attack.getNextValue();
    float min_gain_db = -12.0f;
    float max_gain_db = 12.0f;
    float shelf_gain = juce::Decibels::decibelsToGain(
        min_gain_db + (max_gain_db - min_gain_db) * current_attack * 0.1f
    );
    auto attack_shelf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            processSpec.sampleRate, attack_shelf_cutoff, attack_shelf_q,
            shelf_gain
        );
    *attack_shelf.coefficients = *attack_shelf_coefficients;
}

void HeliosOverdrive::updateGruntFilter()
{
    float current_grunt = grunt.getNextValue();
    float min_gain_db = -12.0f;
    float max_gain_db = 12.0f;
    float gain = juce::Decibels::decibelsToGain(
        min_gain_db + (max_gain_db - min_gain_db) * current_grunt * 0.1f
    );
    auto grunt_coefficients =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            processSpec.sampleRate, grunt_frequency, grunt_filter_q, gain
        );
    *grunt_filter.coefficients = *grunt_coefficients;
}

void HeliosOverdrive::process(juce::AudioBuffer<float>& buffer)
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
        float current_drive = drive.getNextValue();
        if (attack.isSmoothing())
            updateAttackFilter();
        if (grunt.isSmoothing())
            updateGruntFilter();
        float dry = channelData[i];
        float wet = channelData[i];
        applyOverdrive(wet, driveToGain(current_drive));

        float current_level = level.getNextValue();
        float current_mix = mix.getNextValue();

        wet *= current_level;
        channelData[i] = current_mix * wet + (1.0f - current_mix) * dry;
    }
    oversampler2x.processSamplesDown(block);
}

void HeliosOverdrive::applyOverdrive(float& sample, float gain)
{
    float filtered =
        pre_lpf.processSample(pre_hpf.processSample(input_padding * sample));

    // Only drive between low-mids and highs
    float grunted = grunt_filter.processSample(filtered);
    float mids = mid_hpf.processSample(grunted);
    float drived = gain * mids;

    // Filter low mids on "clean" signal
    float lowmids = lowmids_lpf.processSample(filtered);

    // Combine both driven and clean signal before overdrive
    // Add a bit more clean signal to keep lowend
    float input = drived + lowmids;

    // The signal is restricted to values above -0.7V to stay
    // in the linear region of the CMOS inverter.
    float soft_clipped = 0.5f * (diode_plus.processSample(input) + input);
    float hard_clipped = diode_minus.processSample(soft_clipped);
    float distorted =
        dc_hpf.processSample(cmos.processSample(4.5f + hard_clipped));
    float shelved = attack_shelf.processSample(distorted);
    float era = era_mid_scoop.processSample(shelved);

    // Apply three consecutive LPF to smooth out top-end
    float lpfed1 = post_lpf.processSample(era);
    float lpfed2 = post_lpf2.processSample(lpfed1);
    float out = post_lpf3.processSample(lpfed2);
    sample = output_padding * out;
}

#include "helios.h"
#include "../circuits/silicon_diode.h"
#include "../filters/drive_filter.h"
#include <cmath>

#include <juce_dsp/juce_dsp.h>

void HeliosOverdrive::reset()
{
    oversampler2x.reset();
    cmos.reset();
    resetSmoothedValues();
    resetFilters();
    prepareFilters();
}

void HeliosOverdrive::resetFilters()
{
    pre_lpf.reset();
    pre_hpf.reset();
    post_lpf.reset();
    post_lpf2.reset();
    post_lpf3.reset();
    attack_shelf.reset();
    drive_filter.reset();
    grunt_filter.reset();
}

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
    prepareFilters();
}

void HeliosOverdrive::prepareFilters()
{
    attack_shelf.prepare(processSpec);
    updateAttackFilter();

    drive_filter.prepare(processSpec);
    updateDriveFilter();

    grunt_filter.prepare(processSpec);
    updateGruntFilter();

    pre_lpf.prepare(processSpec);
    auto pre_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, pre_lpf_cutoff
        );
    *pre_lpf.coefficients = *pre_lpf_coefficients;

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

void HeliosOverdrive::updateAttackFilter()
{
    float current_attack = attack.getCurrentValue();
    float attack_shelf_q = 0.7f;
    float min_frequency = 1540.0f;
    float max_frequency = 1540.0f;
    float min_gain_db = -24.0f;
    float max_gain_db = 24.0f;
    float shelf_frequency =
        min_frequency + (max_frequency - min_frequency) * current_attack * 0.1f;
    float shelf_gain = juce::Decibels::decibelsToGain(
        min_gain_db + (max_gain_db - min_gain_db) * current_attack * 0.1f
    );

    auto attack_shelf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            processSpec.sampleRate, shelf_frequency, attack_shelf_q, shelf_gain
        );
    *attack_shelf.coefficients = *attack_shelf_coefficients;
}

void HeliosOverdrive::updateDriveFilter()
{
    // float current_grunt = grunt.getCurrentValue();
    float current_drive = drive.getCurrentValue();
    float drive_filter_q = 0.7f;

    // Set the frequency based on the grunt parameter
    float rolloff_frequency = 3200.0f;
    // float min_frequency = 120.0f;
    // float max_frequency = 320.0f;
    // float drive_frequency =
    //     min_frequency + (max_frequency - min_frequency) * current_grunt *
    //     0.1f;
    float drive_frequency = 219.0f;

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

void HeliosOverdrive::updateGruntFilter()
{
    float current_grunt = grunt.getCurrentValue();
    float max_frequency = 700.0f;
    float min_frequency = 2200.0f;
    float grunt_frequency =
        min_frequency + (max_frequency - min_frequency) * current_grunt * 0.1f;
    float max_gain = juce::Decibels::decibelsToGain(-10.0f);
    float min_gain = juce::Decibels::decibelsToGain(-14.0f);
    float gain = min_gain + (max_gain - min_gain) * current_grunt * 0.1f;
    auto grunt_filter_coefficients =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            processSpec.sampleRate, grunt_frequency, 0.707f, gain
        );
    *grunt_filter.coefficients = *grunt_filter_coefficients;
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
        if (attack.isSmoothing())
        {
            attack.getNextValue();
            updateAttackFilter();
        }
        if (grunt.isSmoothing())
        {
            grunt.getNextValue();
            updateGruntFilter();
        }
        if (drive.isSmoothing())
        {
            drive.getNextValue();
            updateDriveFilter();
        }

        float dry = channelData[i];
        float wet = channelData[i];
        applyOverdrive(wet);

        float current_level = level.getNextValue();
        float current_mix = mix.getNextValue();
        wet *= current_level;
        channelData[i] = current_mix * wet + (1.0f - current_mix) * dry;
    }
    oversampler2x.processSamplesDown(block);
}

void HeliosOverdrive::applyOverdrive(float& sample)
{
    float input_padding = juce::Decibels::decibelsToGain(0.0f);
    float raw_input = sample * input_padding;
    float filtered = pre_lpf.processSample(pre_hpf.processSample(raw_input));

    float attacked = attack_shelf.processSample(filtered);
    float drived = drive_filter.processSample(attacked);

    float distorted = cmos.processSample(drived);
    float mid_scooped = grunt_filter.processSample(distorted);

    // Apply three consecutive LPF to smooth out top-end
    float lpfed1 = post_lpf.processSample(mid_scooped);
    float lpfed2 = post_lpf2.processSample(lpfed1);
    float out = post_lpf3.processSample(lpfed2);

    float output_padding = juce::Decibels::decibelsToGain(0.0f);
    sample = output_padding * out;
}

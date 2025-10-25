#include "helios.h"
#include "../circuits/silicon_diode.h"
#include "../filters/drive_filter.h"
#include <cmath>

#include <juce_dsp/juce_dsp.h>

void HeliosOverdrive::reset()
{
    oversampler2x.reset();
    cmos.reset();
    cmos2.reset();
    resetSmoothedValues();
    resetFilters();
    prepareFilters();
}

void HeliosOverdrive::resetFilters()
{
    pre_lpf.reset();
    pre_hpf.reset();

    b3k_attack_shelf.reset();
    b3k_drive_filter.reset();
    b3k_pre_filter_1.reset();
    b3k_pre_filter_2.reset();
    b3k_post_filter_1.reset();
    b3k_post_filter_2.reset();
    b3k_post_filter_3.reset();

    vmt_attack_shelf.reset();
    vmt_drive_filter.reset();
    vmt_pre_filter.reset();
    vmt_post_filter_1.reset();
    vmt_post_filter_2.reset();
    vmt_post_filter_3.reset();
}

void HeliosOverdrive::resetSmoothedValues()
{
    float sample_rate = static_cast<float>(processSpec.sampleRate);
    sample_rate = std::max(1.0f, sample_rate);

    level.reset(sample_rate, smoothing_time);
    level.setCurrentAndTargetValue(raw_level);
    drive.reset(sample_rate, smoothing_time);
    drive.setCurrentAndTargetValue(raw_drive);
    mix.reset(sample_rate, smoothing_time);
    mix.setCurrentAndTargetValue(raw_mix);
    attack.reset(sample_rate, smoothing_time);
    attack.setCurrentAndTargetValue(raw_attack);
    era.reset(sample_rate, smoothing_time);
    era.setCurrentAndTargetValue(raw_era);
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
    vmt_attack_shelf.prepare(processSpec);
    b3k_attack_shelf.prepare(processSpec);
    updateAttackFilter();

    vmt_drive_filter.prepare(processSpec);
    b3k_drive_filter.prepare(processSpec);
    updateDriveFilter();

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

    b3k_pre_filter_1.prepare(processSpec);
    auto b3k_pre_coefficients_1 =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            processSpec.sampleRate, 106.0f, 0.707f
        );
    *b3k_pre_filter_1.coefficients = *b3k_pre_coefficients_1;

    b3k_pre_filter_2.prepare(processSpec);
    auto b3k_pre_coefficients_2 =
        juce::dsp::IIR::Coefficients<float>::makeNotch(
            processSpec.sampleRate, 320.0f, 0.18f
        );
    *b3k_pre_filter_2.coefficients = *b3k_pre_coefficients_2;

    vmt_pre_filter.prepare(processSpec);
    auto vmt_pre_coefficients =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            processSpec.sampleRate, 280.0f, 1.0f,
            juce::Decibels::decibelsToGain(-34.0f)
        );
    *vmt_pre_filter.coefficients = *vmt_pre_coefficients;

    b3k_post_filter_1.prepare(processSpec);
    auto b3k_post_coefficients_1 =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            processSpec.sampleRate, 360.0f, 0.707f,
            juce::Decibels::decibelsToGain(-15.0f)
        );
    *b3k_post_filter_1.coefficients = *b3k_post_coefficients_1;

    b3k_post_filter_2.prepare(processSpec);
    auto b3k_post_coefficients_2 =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, b3k_post_lpf_cutoff_2, b3k_post_lpf_q_2
        );
    *b3k_post_filter_2.coefficients = *b3k_post_coefficients_2;

    b3k_post_filter_3.prepare(processSpec);
    auto b3k_post_coefficients_3 =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, b3k_post_lpf_cutoff_3, b3k_post_lpf_q_3
        );
    *b3k_post_filter_3.coefficients = *b3k_post_coefficients_3;

    vmt_post_filter_1.prepare(processSpec);
    auto vmt_post_coefficients_1 =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            processSpec.sampleRate, 700.0f, 0.707f,
            juce::Decibels::decibelsToGain(-6.0f)
        );
    *vmt_post_filter_1.coefficients = *vmt_post_coefficients_1;

    vmt_post_filter_2.prepare(processSpec);
    auto vmt_post_coefficients_2 =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, vmt_post_lpf_cutoff_2, vmt_post_lpf_q_2
        );
    *vmt_post_filter_2.coefficients = *vmt_post_coefficients_2;

    vmt_post_filter_3.prepare(processSpec);
    auto vmt_post_coefficients_3 =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            processSpec.sampleRate, vmt_post_lpf_cutoff_3, vmt_post_lpf_q_3
        );
    *vmt_post_filter_3.coefficients = *vmt_post_coefficients_3;
}

void HeliosOverdrive::updateAttackFilter()
{
    float current_attack = attack.getCurrentValue();
    float attack_shelf_q = 0.7f;
    float min_frequency = 1540.0f;
    float max_frequency = 1540.0f;
    float min_gain_db = -18.0f;
    float max_gain_db = 18.0f;
    float shelf_frequency =
        min_frequency + (max_frequency - min_frequency) * current_attack * 0.1f;
    float shelf_gain = juce::Decibels::decibelsToGain(
        min_gain_db + (max_gain_db - min_gain_db) * current_attack * 0.1f
    );

    auto attack_shelf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            processSpec.sampleRate, shelf_frequency, attack_shelf_q, shelf_gain
        );
    *vmt_attack_shelf.coefficients = *attack_shelf_coefficients;
    *b3k_attack_shelf.coefficients = *attack_shelf_coefficients;
}

void HeliosOverdrive::updateDriveFilter()
{
    // float current_grunt = grunt.getCurrentValue();
    float current_drive = drive.getCurrentValue();
    float drive_filter_q = 0.7f;

    // Set the frequency based on the grunt parameter
    float rolloff_frequency = 3200.0f;
    float drive_frequency = 209.0f;

    // Set the gain based on the drive parameter
    float min_gain_db = 0.0f;
    float max_gain_db = 48.0f;
    float drive_filter_gain = juce::Decibels::decibelsToGain(
        min_gain_db + (max_gain_db - min_gain_db) * current_drive * 0.1f
    );

    auto drive_filter_coefficients = makeDriveFilter(
        processSpec.sampleRate, drive_frequency, rolloff_frequency,
        drive_filter_gain
    );
    *vmt_drive_filter.coefficients = *drive_filter_coefficients;
    *b3k_drive_filter.coefficients = *drive_filter_coefficients;
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
    float current_era = 0.1f * era.getNextValue();
    float raw_input = sample;
    float filtered = pre_lpf.processSample(pre_hpf.processSample(raw_input));

    float vmt_pre = vmt_pre_filter.processSample(filtered);
    float vmt_attacked = vmt_attack_shelf.processSample(vmt_pre);
    float vmt_drived = vmt_drive_filter.processSample(vmt_attacked);
    float vmt_distorted = cmos.processSample(vmt_drived);
    float vmt_post = vmt_post_filter_1.processSample(vmt_distorted);
    float vmt_post_2 = vmt_post_filter_2.processSample(vmt_post);
    float vmt_post_3 = vmt_post_filter_3.processSample(vmt_post_2);

    float b3k_pre_1 = b3k_pre_filter_1.processSample(filtered);
    float b3k_pre_2 = b3k_pre_filter_2.processSample(b3k_pre_1);
    float b3k_attacked = b3k_attack_shelf.processSample(b3k_pre_2);
    float b3k_drived = b3k_drive_filter.processSample(b3k_attacked);
    float b3k_distorted = cmos2.processSample(b3k_drived);
    float b3k_post = b3k_post_filter_1.processSample(b3k_distorted);
    float b3k_post_2 = b3k_post_filter_2.processSample(b3k_post);
    float b3k_post_3 = b3k_post_filter_3.processSample(b3k_post_2);

    float post = vmt_post_3 * (1.0f - current_era) + b3k_post_3 * current_era;

    sample = post;
}

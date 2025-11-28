#include "helios.h"
#include "../filters/drive_filter.h"
#include <cmath>

#include <juce_dsp/juce_dsp.h>

void HeliosOverdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= (int)(2 * std::pow(2, oversampling_index));
    process_spec = oversampled_spec;

    for (auto oversampler : oversamplers)
    {
        oversampler->reset();
        oversampler->initProcessing(static_cast<size_t>(spec.maximumBlockSize));
    }
    vmt_buffer.setSize(
        (int)process_spec.numChannels, (int)process_spec.maximumBlockSize * 16,
        false, false, true
    );
    mu_amp.prepare(process_spec);
    cmos.prepare();
    resetSmoothedValues();
    prepareFilters();
}

void HeliosOverdrive::reset()
{
    oversampler2x.reset();
    oversampler4x.reset();
    oversampler8x.reset();
    mu_amp.reset();
    cmos.reset();
    resetSmoothedValues();
    resetFilters();
    prepareFilters();
}

void HeliosOverdrive::resetFilters()
{
    vmt_pre_lpf.reset();
    vmt_pre_hpf.reset();
    vmt_attack_shelf.reset();
    vmt_grunt_filter.reset();
    vmt_era_filter.reset();
}

void HeliosOverdrive::resetSmoothedValues()
{
    level.reset(process_spec.sampleRate, smoothing_time);
    level.setCurrentAndTargetValue(raw_level);
    drive.reset(process_spec.sampleRate, smoothing_time);
    drive.setCurrentAndTargetValue(raw_drive);
    mix.reset(process_spec.sampleRate, smoothing_time);
    mix.setCurrentAndTargetValue(raw_mix);
    attack.reset(process_spec.sampleRate, smoothing_time);
    attack.setCurrentAndTargetValue(raw_attack);
    grunt.reset(process_spec.sampleRate, smoothing_time);
    grunt.setCurrentAndTargetValue(raw_grunt);
    era.reset(process_spec.sampleRate, smoothing_time);
    era.setCurrentAndTargetValue(raw_era);
}

void HeliosOverdrive::prepareFilters()
{
    vmt_attack_shelf.prepare(process_spec);
    updateAttackFilter();

    vmt_drive_filter.prepare(process_spec);
    updateDriveFilter();

    vmt_grunt_filter.prepare(process_spec);
    updateGruntFilter();

    vmt_era_filter.prepare(process_spec);
    updateEraFilter();

    vmt_pre_lpf.prepare(process_spec);
    auto vmt_pre_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, pre_lpf_cutoff
        );
    *vmt_pre_lpf.coefficients = *vmt_pre_lpf_coefficients;

    vmt_pre_hpf.prepare(process_spec);
    auto vmt_pre_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            process_spec.sampleRate, pre_hpf_cutoff
        );
    *vmt_pre_hpf.coefficients = *vmt_pre_hpf_coefficients;

    vmt_pre_filter.prepare(process_spec);
    auto vmt_pre_coefficients = juce::dsp::IIR::Coefficients<float>::makeNotch(
        process_spec.sampleRate, 288.0f, 0.175f
    );
    *vmt_pre_filter.coefficients = *vmt_pre_coefficients;

    vmt_pre_filter_2.prepare(process_spec);
    auto vmt_pre_coefficients_2 =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            process_spec.sampleRate, 65.0f,
            juce::Decibels::decibelsToGain(-2.5f), 0.45f
        );
    *vmt_pre_filter_2.coefficients = *vmt_pre_coefficients_2;

    vmt_pre_filter_3.prepare(process_spec);
    auto vmt_pre_coefficients_3 =
        juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            process_spec.sampleRate, 1539.0f,
            juce::Decibels::decibelsToGain(-8.0f), 0.40f
        );
    *vmt_pre_filter_3.coefficients = *vmt_pre_coefficients_3;

    vmt_post_filter_2.prepare(process_spec);
    auto vmt_post_coefficients_2 =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, vmt_post_lpf_cutoff_2, vmt_post_lpf_q_2
        );
    *vmt_post_filter_2.coefficients = *vmt_post_coefficients_2;

    vmt_post_filter_3.prepare(process_spec);
    auto vmt_post_coefficients_3 =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, vmt_post_lpf_cutoff_3, vmt_post_lpf_q_3
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
            process_spec.sampleRate, shelf_frequency, attack_shelf_q, shelf_gain
        );
    *vmt_attack_shelf.coefficients = *attack_shelf_coefficients;
}

void HeliosOverdrive::updateGruntFilter()
{
    float current_grunt = grunt.getCurrentValue();
    float min_frequency = 120.0f;
    float max_frequency = 506.0f;
    float frequency = min_frequency + (max_frequency - min_frequency) *
                                          (1.0f - current_grunt * 0.1f);

    auto grunt_coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        process_spec.sampleRate, frequency
    );
    *vmt_grunt_filter.coefficients = *grunt_coefficients;
}

void HeliosOverdrive::updateEraFilter()
{
    float current_era = era.getCurrentValue();
    float era = 1.0f - 0.099f * current_era;

    float min_frequency = 700.0f;
    float frequency = min_frequency * std::exp(1.21533f * era * era);

    float min_q = 0.45f;
    float max_q = 0.25f;
    float q = min_q + (max_q - min_q) * era;

    // auto era_coefficients =
    // juce::dsp::IIR::Coefficients<float>::makePeakFilter(
    //     process_spec.sampleRate, frequency, q,
    //     juce::Decibels::decibelsToGain(-10.0f - 4.3f * era)
    // );
    auto era_coefficients = juce::dsp::IIR::Coefficients<float>::makeNotch(
        process_spec.sampleRate, frequency, q
    );
    *vmt_era_filter.coefficients = *era_coefficients;
}

void HeliosOverdrive::updateDriveFilter()
{
    float current_drive = drive.getCurrentValue();

    float rolloff_frequency = 5000.0f;
    float drive_frequency = 30.0f;

    // Set the gain based on the drive parameter
    float min_gain_db = 0.0f;
    float max_gain_db = 30.0f;
    float drive_filter_gain = juce::Decibels::decibelsToGain(
        min_gain_db + (max_gain_db - min_gain_db) * current_drive * 0.1f
    );

    auto drive_filter_coefficients = makeDriveFilter(
        (float)process_spec.sampleRate, drive_frequency, rolloff_frequency,
        drive_filter_gain
    );
    *vmt_drive_filter.coefficients = *drive_filter_coefficients;
}

void HeliosOverdrive::process(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    auto& block = context.getOutputBlock();
    auto* oversampler = oversamplers[oversampling_index];
    auto oversampled_block = oversampler->processSamplesUp(block);

    const size_t num_samples = oversampled_block.getNumSamples();

    if (attack.isSmoothing())
    {
        attack.skip((int)num_samples);
        updateAttackFilter();
    }
    if (grunt.isSmoothing())
    {
        grunt.skip((int)num_samples);
        updateGruntFilter();
    }
    if (drive.isSmoothing())
    {
        drive.skip((int)num_samples);
        updateDriveFilter();
    }
    if (era.isSmoothing())
    {
        era.skip((int)num_samples);
        updateEraFilter();
    }

    juce::dsp::AudioBlock<float> vmt_block(vmt_buffer);
    auto vmt_sub = vmt_block.getSubBlock(0, num_samples);
    vmt_sub.copyFrom(oversampled_block);

    auto vmt_context = juce::dsp::ProcessContextReplacing<float>(vmt_sub);
    processVMT(vmt_context);

    auto* ch = oversampled_block.getChannelPointer(0);
    auto* vmt = vmt_sub.getChannelPointer(0);

    for (size_t i = 0; i < num_samples; ++i)
    {
        float current_level = level.getNextValue();
        float current_mix = mix.getNextValue();

        float dry = ch[i];
        float od = vmt[i] * current_level;
        ch[i] = current_mix * od + (1.0f - current_mix) * dry;
    }
    oversampler->processSamplesDown(block);
}

void HeliosOverdrive::processVMT(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    vmt_pre_lpf.process(context);
    mu_amp.process(context);
    vmt_pre_hpf.process(context);
    vmt_pre_filter.process(context);
    vmt_pre_filter_2.process(context);
    vmt_pre_filter_3.process(context);
    vmt_attack_shelf.process(context);
    vmt_drive_filter.process(context);
    vmt_grunt_filter.process(context);
    cmos.process(context);
    vmt_era_filter.process(context);
    vmt_post_filter_2.process(context);
    vmt_post_filter_3.process(context);
}

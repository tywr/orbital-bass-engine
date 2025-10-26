#include "helios.h"
#include "../circuits/silicon_diode.h"
#include "../filters/drive_filter.h"
#include <cmath>

#include <juce_dsp/juce_dsp.h>

void HeliosOverdrive::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= 2;
    process_spec = oversampled_spec;

    oversampler2x.reset();
    oversampler2x.initProcessing(static_cast<size_t>(spec.maximumBlockSize));

    vmt_buffer.setSize(
        (int)process_spec.numChannels, (int)process_spec.maximumBlockSize * 4,
        false, false, true
    );
    b3k_buffer.setSize(
        (int)process_spec.numChannels, (int)process_spec.maximumBlockSize * 4,
        false, false, true
    );

    resetSmoothedValues();
    prepareFilters();
}

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

    b3k_pre_lpf.reset();
    b3k_pre_hpf.reset();
    b3k_attack_shelf.reset();
    b3k_drive_filter.reset();
    b3k_pre_filter_1.reset();
    b3k_pre_filter_2.reset();
    b3k_post_filter_1.reset();
    b3k_post_filter_2.reset();
    b3k_post_filter_3.reset();

    vmt_pre_lpf.reset();
    vmt_pre_hpf.reset();
    vmt_attack_shelf.reset();
    vmt_drive_filter.reset();
    vmt_pre_filter.reset();
    vmt_post_filter_1.reset();
    vmt_post_filter_2.reset();
    vmt_post_filter_3.reset();
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
    era.reset(process_spec.sampleRate, smoothing_time);
    era.setCurrentAndTargetValue(raw_era);
}

void HeliosOverdrive::prepareFilters()
{
    vmt_attack_shelf.prepare(process_spec);
    b3k_attack_shelf.prepare(process_spec);
    updateAttackFilter();

    vmt_drive_filter.prepare(process_spec);
    b3k_drive_filter.prepare(process_spec);
    updateDriveFilter();

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

    b3k_pre_lpf.prepare(process_spec);
    auto b3k_pre_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, pre_lpf_cutoff
        );
    *b3k_pre_lpf.coefficients = *b3k_pre_lpf_coefficients;

    b3k_pre_hpf.prepare(process_spec);
    auto b3k_pre_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            process_spec.sampleRate, pre_hpf_cutoff
        );
    *b3k_pre_hpf.coefficients = *b3k_pre_hpf_coefficients;

    b3k_pre_filter_1.prepare(process_spec);
    auto b3k_pre_coefficients_1 =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            process_spec.sampleRate, 106.0f, 0.707f
        );
    *b3k_pre_filter_1.coefficients = *b3k_pre_coefficients_1;

    b3k_pre_filter_2.prepare(process_spec);
    auto b3k_pre_coefficients_2 =
        juce::dsp::IIR::Coefficients<float>::makeNotch(
            process_spec.sampleRate, 320.0f, 0.18f
        );
    *b3k_pre_filter_2.coefficients = *b3k_pre_coefficients_2;

    vmt_pre_filter.prepare(process_spec);
    auto vmt_pre_coefficients =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            process_spec.sampleRate, 280.0f, 1.0f,
            juce::Decibels::decibelsToGain(-34.0f)
        );
    *vmt_pre_filter.coefficients = *vmt_pre_coefficients;

    b3k_post_filter_1.prepare(process_spec);
    auto b3k_post_coefficients_1 =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            process_spec.sampleRate, 360.0f, 0.707f,
            juce::Decibels::decibelsToGain(-15.0f)
        );
    *b3k_post_filter_1.coefficients = *b3k_post_coefficients_1;

    b3k_post_filter_2.prepare(process_spec);
    auto b3k_post_coefficients_2 =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, b3k_post_lpf_cutoff_2, b3k_post_lpf_q_2
        );
    *b3k_post_filter_2.coefficients = *b3k_post_coefficients_2;

    b3k_post_filter_3.prepare(process_spec);
    auto b3k_post_coefficients_3 =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, b3k_post_lpf_cutoff_3, b3k_post_lpf_q_3
        );
    *b3k_post_filter_3.coefficients = *b3k_post_coefficients_3;

    vmt_post_filter_1.prepare(process_spec);
    auto vmt_post_coefficients_1 =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            process_spec.sampleRate, 700.0f, 0.707f,
            juce::Decibels::decibelsToGain(-6.0f)
        );
    *vmt_post_filter_1.coefficients = *vmt_post_coefficients_1;

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
    *b3k_attack_shelf.coefficients = *attack_shelf_coefficients;
}

void HeliosOverdrive::updateDriveFilter()
{
    float current_drive = drive.getCurrentValue();

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
        (float)process_spec.sampleRate, drive_frequency, rolloff_frequency,
        drive_filter_gain
    );
    *vmt_drive_filter.coefficients = *drive_filter_coefficients;
    *b3k_drive_filter.coefficients = *drive_filter_coefficients;
}

void HeliosOverdrive::process(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    auto& block = context.getOutputBlock();
    auto oversampled_block = oversampler2x.processSamplesUp(block);

    const size_t num_samples = oversampled_block.getNumSamples();

    if (attack.isSmoothing())
    {
        attack.skip((int)num_samples);
        updateAttackFilter();
    }
    if (drive.isSmoothing())
    {
        drive.skip((int)num_samples);
        updateDriveFilter();
    }

    juce::dsp::AudioBlock<float> vmt_block(vmt_buffer);
    juce::dsp::AudioBlock<float> b3k_block(b3k_buffer);
    auto vmt_sub = vmt_block.getSubBlock(0, num_samples);
    auto b3k_sub = b3k_block.getSubBlock(0, num_samples);
    vmt_sub.copyFrom(oversampled_block);
    b3k_sub.copyFrom(oversampled_block);

    auto vmt_context = juce::dsp::ProcessContextReplacing<float>(vmt_sub);
    processVMT(vmt_context);

    auto b3k_context = juce::dsp::ProcessContextReplacing<float>(b3k_sub);
    processB3K(b3k_context);

    auto* ch = oversampled_block.getChannelPointer(0);
    auto* vmt = vmt_sub.getChannelPointer(0);
    auto* b3k = b3k_sub.getChannelPointer(0);

    for (size_t i = 0; i < num_samples; ++i)
    {
        float current_era = 0.1f * era.getNextValue();
        float current_level = level.getNextValue();
        float current_mix = mix.getNextValue();

        float dry = ch[i];
        float od = (vmt[i] * (1.0f - current_era) + b3k[i] * current_era) *
                   current_level;
        ch[i] = current_mix * od + (1.0f - current_mix) * dry;
    }
    oversampler2x.processSamplesDown(block);
}

void HeliosOverdrive::processVMT(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    vmt_pre_lpf.process(context);
    vmt_pre_hpf.process(context);
    vmt_pre_filter.process(context);
    vmt_attack_shelf.process(context);
    vmt_drive_filter.process(context);
    cmos.process(context);
    vmt_post_filter_1.process(context);
    vmt_post_filter_2.process(context);
    vmt_post_filter_3.process(context);
}

void HeliosOverdrive::processB3K(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    b3k_pre_lpf.process(context);
    b3k_pre_hpf.process(context);
    b3k_pre_filter_1.process(context);
    b3k_pre_filter_2.process(context);
    b3k_attack_shelf.process(context);
    b3k_drive_filter.process(context);
    cmos2.process(context);
    b3k_post_filter_1.process(context);
    b3k_post_filter_2.process(context);
    b3k_post_filter_3.process(context);
}

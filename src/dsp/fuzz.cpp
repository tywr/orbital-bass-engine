#include "fuzz.h"
#include "filters/drive_filter.h"
#include <cmath>

#include <juce_dsp/juce_dsp.h>

void Fuzz::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec oversampled_spec = spec;
    oversampled_spec.sampleRate *= 4;
    process_spec = oversampled_spec;

    diode_pair.prepare(process_spec);

    oversampler.reset();
    oversampler.initProcessing(static_cast<size_t>(spec.maximumBlockSize));

    noise_gate.prepare(process_spec);
    noise_gate.setThreshold(-40.0f);

    square_buffer.setSize(
        (int)process_spec.numChannels,
        (int)process_spec.maximumBlockSize * 4 * 4, false, false, true
    );
    reset();
}

void Fuzz::reset()
{
    oversampler.reset();
    resetSmoothedValues();
    resetFilters();
    prepareFilters();
}

void Fuzz::resetFilters()
{

    square_pre_lpf.reset();
    square_pre_hpf.reset();
    square_post_filter_1.reset();
    square_post_filter_2.reset();
    square_post_filter_3.reset();
}

void Fuzz::resetSmoothedValues()
{
    level.reset(process_spec.sampleRate, smoothing_time);
    level.setCurrentAndTargetValue(raw_level);
    sustain.reset(process_spec.sampleRate, smoothing_time);
    sustain.setCurrentAndTargetValue(raw_sustain);
    mix.reset(process_spec.sampleRate, smoothing_time);
    mix.setCurrentAndTargetValue(raw_mix);
    tone.reset(process_spec.sampleRate, smoothing_time);
    tone.setCurrentAndTargetValue(raw_tone);
}

void Fuzz::prepareFilters()
{

    tone_filter.prepare(process_spec);
    drive_filter_1.prepare(process_spec);
    drive_filter_2.prepare(process_spec);
    updateToneFilter();
    updateDriveFilters();

    square_pre_lpf.prepare(process_spec);
    auto square_pre_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, pre_lpf_cutoff
        );
    *square_pre_lpf.coefficients = *square_pre_lpf_coefficients;

    square_pre_hpf.prepare(process_spec);
    auto square_pre_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            process_spec.sampleRate, pre_hpf_cutoff
        );
    *square_pre_hpf.coefficients = *square_pre_hpf_coefficients;

    square_post_filter_1.prepare(process_spec);
    auto square_post_coefficients_1 =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, square_post_lpf_cutoff_1,
            square_post_lpf_q_1
        );
    *square_post_filter_1.coefficients = *square_post_coefficients_1;

    square_post_filter_2.prepare(process_spec);
    auto square_post_coefficients_2 =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, square_post_lpf_cutoff_2,
            square_post_lpf_q_2
        );
    *square_post_filter_2.coefficients = *square_post_coefficients_2;

    square_post_filter_3.prepare(process_spec);
    auto square_post_coefficients_3 =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, square_post_lpf_cutoff_3,
            square_post_lpf_q_3
        );
    *square_post_filter_3.coefficients = *square_post_coefficients_3;
}

void Fuzz::updateToneFilter()
{
    float current_tone = tone.getNextValue();
    float min_gain = juce::Decibels::decibelsToGain(-12.0f);
    float max_gain = juce::Decibels::decibelsToGain(0.0f);
    float tone_gain = min_gain + (max_gain - min_gain) * (0.1f * current_tone);

    auto tone_coefficients =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            process_spec.sampleRate, 800.0f, 0.707f, tone_gain
        );
    *tone_filter.coefficients = *tone_coefficients;
}

void Fuzz::updateDriveFilters()
{
    float current_sustain = sustain.getNextValue();
    float min_gain = juce::Decibels::decibelsToGain(14.0f);
    float max_gain = juce::Decibels::decibelsToGain(20.0f);
    float drive_gain =
        min_gain + (max_gain - min_gain) * (0.1f * current_sustain);

    auto drive_coefficients_1 =
        makeDriveFilter(process_spec.sampleRate, 25.0f, 3000.0f, drive_gain);
    *drive_filter_1.coefficients = *drive_coefficients_1;

    auto drive_coefficients_2 =
        makeDriveFilter(process_spec.sampleRate, 25.0f, 3000.0f, min_gain);
    *drive_filter_2.coefficients = *drive_coefficients_2;
}

void Fuzz::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& block = context.getOutputBlock();
    auto oversampled_block = oversampler.processSamplesUp(block);

    const size_t num_samples = oversampled_block.getNumSamples();

    if (tone.isSmoothing())
    {
        tone.skip((int)num_samples);
        updateToneFilter();
    }

    if (sustain.isSmoothing())
    {
        sustain.skip((int)num_samples);
        updateDriveFilters();
    }

    juce::dsp::AudioBlock<float> square_block(square_buffer);
    auto square_sub = square_block.getSubBlock(0, num_samples);
    square_sub.copyFrom(oversampled_block);

    auto square_context = juce::dsp::ProcessContextReplacing<float>(square_sub);
    processSquare(square_context);

    auto* ch = oversampled_block.getChannelPointer(0);
    auto* square = square_sub.getChannelPointer(0);

    for (size_t i = 0; i < num_samples; ++i)
    {
        float current_level = level.getNextValue();
        float current_mix = mix.getNextValue();

        float dry = ch[i];
        float od = square[i] * current_level;
        ch[i] = current_mix * od + (1.0f - current_mix) * dry;
    }
    oversampler.processSamplesDown(block);
}

void Fuzz::processSquare(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{

    auto& block = context.getOutputBlock();
    square_pre_lpf.process(context);
    square_pre_hpf.process(context);

    noise_gate.process(context);
    drive_filter_1.process(context);
    diode_pair.process(context);
    drive_filter_2.process(context);
    diode_pair.process(context);
    block.multiplyBy(juce::Decibels::decibelsToGain(-16.0f));

    tone_filter.process(context);

    square_post_filter_1.process(context);
    square_post_filter_2.process(context);
    square_post_filter_3.process(context);
}

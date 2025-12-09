#include "eq.h"
#include <algorithm>

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

void EQ::reset()
{
    resetSmoothedValues();
    low_shelf_filter.reset();
    low_mid_filter.reset();
    high_mid_filter.reset();
    high_shelf_filter.reset();
    lpf_filter_1.reset();
    lpf_filter_2.reset();
    setCoefficients();
}

void EQ::resetSmoothedValues()
{

    float sample_rate = static_cast<float>(processSpec.sampleRate);
    sample_rate = std::max(1.0f, sample_rate);

    low_shelf_gain.reset(sample_rate, smoothing_time);
    low_shelf_gain.setCurrentAndTargetValue(raw_low_shelf_gain);
    low_shelf_freq.reset(sample_rate, smoothing_time);
    low_shelf_freq.setCurrentAndTargetValue(raw_low_shelf_freq);

    low_mid_freq.reset(sample_rate, smoothing_time);
    low_mid_freq.setCurrentAndTargetValue(raw_low_mid_freq);
    low_mid_q.reset(sample_rate, smoothing_time);
    low_mid_q.setCurrentAndTargetValue(raw_low_mid_q);
    low_mid_gain.reset(sample_rate, smoothing_time);
    low_mid_gain.setCurrentAndTargetValue(raw_low_mid_gain);

    high_mid_freq.reset(sample_rate, smoothing_time);
    high_mid_freq.setCurrentAndTargetValue(raw_high_mid_freq);
    high_mid_q.reset(sample_rate, smoothing_time);
    high_mid_q.setCurrentAndTargetValue(raw_high_mid_q);
    high_mid_gain.reset(sample_rate, smoothing_time);
    high_mid_gain.setCurrentAndTargetValue(raw_high_mid_gain);

    high_shelf_gain.reset(sample_rate, smoothing_time);
    high_shelf_gain.setCurrentAndTargetValue(raw_high_shelf_gain);
    high_shelf_freq.reset(sample_rate, smoothing_time);
    high_shelf_freq.setCurrentAndTargetValue(raw_high_shelf_freq);

    lpf_frequency.reset(sample_rate, smoothing_time);
    lpf_frequency.setCurrentAndTargetValue(raw_lpf_frequency);
}

void EQ::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
    resetSmoothedValues();
    low_shelf_filter.prepare(spec);
    low_mid_filter.prepare(spec);
    high_mid_filter.prepare(spec);
    high_shelf_filter.prepare(spec);
    lpf_filter_1.prepare(spec);
    lpf_filter_2.prepare(spec);
    reset();
}

void EQ::setCoefficients()
{
    low_shelf_gain.skip((int)processSpec.maximumBlockSize);
    low_shelf_freq.skip((int)processSpec.maximumBlockSize);
    low_mid_freq.skip((int)processSpec.maximumBlockSize);
    low_mid_q.skip((int)processSpec.maximumBlockSize);
    low_mid_gain.skip((int)processSpec.maximumBlockSize);
    high_mid_freq.skip((int)processSpec.maximumBlockSize);
    high_mid_q.skip((int)processSpec.maximumBlockSize);
    high_mid_gain.skip((int)processSpec.maximumBlockSize);
    high_shelf_gain.skip((int)processSpec.maximumBlockSize);
    high_shelf_freq.skip((int)processSpec.maximumBlockSize);
    lpf_frequency.skip((int)processSpec.maximumBlockSize);

    float current_low_shelf_gain = low_shelf_gain.getCurrentValue();
    float current_low_shelf_freq = low_shelf_freq.getCurrentValue();
    auto low_shelf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            processSpec.sampleRate, current_low_shelf_freq, low_shelf_q,
            current_low_shelf_gain
        );
    *low_shelf_filter.coefficients = *low_shelf_coefficients;

    float current_low_mid_freq = low_mid_freq.getCurrentValue();
    float current_low_mid_q = low_mid_q.getCurrentValue();
    float current_low_mid_gain = low_mid_gain.getCurrentValue();
    auto low_mid_coefficients =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            processSpec.sampleRate, current_low_mid_freq, current_low_mid_q,
            current_low_mid_gain
        );
    *low_mid_filter.coefficients = *low_mid_coefficients;

    float current_high_mid_freq = high_mid_freq.getCurrentValue();
    float current_high_mid_q = high_mid_q.getCurrentValue();
    float current_high_mid_gain = high_mid_gain.getCurrentValue();
    auto high_mid_coefficients =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            processSpec.sampleRate, current_high_mid_freq, current_high_mid_q,
            current_high_mid_gain
        );
    *high_mid_filter.coefficients = *high_mid_coefficients;

    float current_high_shelf_gain = high_shelf_gain.getCurrentValue();
    float current_high_shelf_freq = high_shelf_freq.getCurrentValue();
    auto high_shelf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            processSpec.sampleRate, current_high_shelf_freq, high_shelf_q,
            current_high_shelf_gain
        );
    *high_shelf_filter.coefficients = *high_shelf_coefficients;

    float current_lpf_frequency = lpf_frequency.getCurrentValue();
    auto lpf_coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        processSpec.sampleRate, current_lpf_frequency
    );
    *lpf_filter_1.coefficients = *lpf_coefficients;
    *lpf_filter_2.coefficients = *lpf_coefficients;
}

void EQ::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    if (low_shelf_gain.isSmoothing() || low_shelf_freq.isSmoothing() ||
        low_mid_freq.isSmoothing() || low_mid_q.isSmoothing() ||
        low_mid_gain.isSmoothing() || high_mid_freq.isSmoothing() ||
        high_mid_q.isSmoothing() || high_mid_gain.isSmoothing() ||
        high_shelf_gain.isSmoothing() || high_shelf_freq.isSmoothing() ||
        lpf_frequency.isSmoothing())
        setCoefficients();

    low_shelf_filter.process(context);
    low_mid_filter.process(context);
    high_mid_filter.process(context);
    high_shelf_filter.process(context);
    lpf_filter_1.process(context);
    lpf_filter_2.process(context);
}

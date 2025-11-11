#include "eq.h"
#include <algorithm>

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

void EQ::reset()
{
    resetSmoothedValues();
    b80_filter.reset();
    b250_filter.reset();
    b500_filter.reset();
    b1500_filter.reset();
    b3000_filter.reset();
    b5000_filter.reset();
    lpf_filter.reset();
    setCoefficients();
}

void EQ::resetSmoothedValues()
{

    float sample_rate = static_cast<float>(processSpec.sampleRate);
    sample_rate = std::max(1.0f, sample_rate);

    b80_gain.reset(sample_rate, smoothing_time);
    b80_gain.setCurrentAndTargetValue(raw_b80_gain);
    b250_gain.reset(sample_rate, smoothing_time);
    b250_gain.setCurrentAndTargetValue(raw_b250_gain);
    b500_gain.reset(sample_rate, smoothing_time);
    b500_gain.setCurrentAndTargetValue(raw_b500_gain);
    b1500_gain.reset(sample_rate, smoothing_time);
    b1500_gain.setCurrentAndTargetValue(raw_b1500_gain);
    b3000_gain.reset(sample_rate, smoothing_time);
    b3000_gain.setCurrentAndTargetValue(raw_b3000_gain);
    b5000_gain.reset(sample_rate, smoothing_time);
    b5000_gain.setCurrentAndTargetValue(raw_b5000_gain);
    lpf_frequency.reset(sample_rate, smoothing_time);
    lpf_frequency.setCurrentAndTargetValue(raw_lpf_frequency);
}

void EQ::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
    resetSmoothedValues();
    b80_filter.prepare(spec);
    b250_filter.prepare(spec);
    b500_filter.prepare(spec);
    b1500_filter.prepare(spec);
    b3000_filter.prepare(spec);
    b5000_filter.prepare(spec);
    lpf_filter.prepare(spec);
}

void EQ::setCoefficients()
{
    if (b80_gain.isSmoothing())
    {
        float current_b80_gain = b80_gain.getNextValue();
        auto b80_coefficients =
            juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                processSpec.sampleRate, b80_freq, b80_q, current_b80_gain
            );
        *b80_filter.coefficients = *b80_coefficients;
    }
    else if (b250_gain.isSmoothing())
    {
        float current_b250_gain = b250_gain.getNextValue();
        auto b250_coefficients =
            juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                processSpec.sampleRate, b250_freq, b250_q, current_b250_gain
            );
        *b250_filter.coefficients = *b250_coefficients;
    }
    else if (b500_gain.isSmoothing())
    {
        float current_b500_gain = b500_gain.getNextValue();
        auto b500_coefficients =
            juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                processSpec.sampleRate, b500_freq, b500_q, current_b500_gain
            );
        *b500_filter.coefficients = *b500_coefficients;
    }
    else if (b1500_gain.isSmoothing())
    {
        float current_b1500_gain = b1500_gain.getNextValue();
        auto b1500_coefficients =
            juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                processSpec.sampleRate, b1500_freq, b1500_q, current_b1500_gain
            );
        *b1500_filter.coefficients = *b1500_coefficients;
    }
    else if (b3000_gain.isSmoothing())
    {
        float current_b3000_gain = b3000_gain.getNextValue();
        auto b3000_coefficients =
            juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                processSpec.sampleRate, b3000_freq, b3000_q, current_b3000_gain
            );
        *b3000_filter.coefficients = *b3000_coefficients;
    }
    else if (b5000_gain.isSmoothing())
    {
        float current_b5000_gain = b5000_gain.getNextValue();
        auto b5000_coefficients =
            juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                processSpec.sampleRate, b5000_freq, b5000_q, current_b5000_gain
            );
        *b5000_filter.coefficients = *b5000_coefficients;
    }
    else if (lpf_frequency.isSmoothing())
    {
        float current_lpf_frequency = lpf_frequency.getNextValue();
        auto lpf_coefficients =
            juce::dsp::IIR::Coefficients<float>::makeLowPass(
                processSpec.sampleRate, current_lpf_frequency
            );
        *lpf_filter.coefficients = *lpf_coefficients;
    }
}

void EQ::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    setCoefficients();
    b80_filter.process(context);
    b250_filter.process(context);
    b500_filter.process(context);
    b1500_filter.process(context);
    b3000_filter.process(context);
    b5000_filter.process(context);
    lpf_filter.process(context);
}

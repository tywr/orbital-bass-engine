#include "amp_eq.h"
#include <algorithm>

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

void AmpEQ::reset()
{
    resetSmoothedValues();
    bass_shelf.reset();
    low_mid_peak.reset();
    high_mid_peak.reset();
    treble_shelf.reset();
    setCoefficients();
}

void AmpEQ::resetSmoothedValues()
{

    float sample_rate = static_cast<float>(processSpec.sampleRate);
    sample_rate = std::max(1.0f, sample_rate);

    bass_gain.reset(sample_rate, smoothing_time);
    bass_gain.setCurrentAndTargetValue(raw_bass_gain);
    low_mid_gain.reset(sample_rate, smoothing_time);
    low_mid_gain.setCurrentAndTargetValue(raw_low_mid_gain);
    high_mid_gain.reset(sample_rate, smoothing_time);
    high_mid_gain.setCurrentAndTargetValue(raw_high_mid_gain);
    treble_gain.reset(sample_rate, smoothing_time);
    treble_gain.setCurrentAndTargetValue(raw_treble_gain);
}

void AmpEQ::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
    resetSmoothedValues();
    bass_shelf.prepare(processSpec);
    low_mid_peak.prepare(processSpec);
    high_mid_peak.prepare(processSpec);
    treble_shelf.prepare(processSpec);
}

void AmpEQ::setCoefficients()
{
    if (bass_gain.isSmoothing())
    {
        float current_bass_gain = bass_gain.getNextValue();
        auto bass_shelf_coefficients =
            juce::dsp::IIR::Coefficients<float>::makeLowShelf(
                processSpec.sampleRate, bass_shelf_frequency, bass_shelf_q,
                current_bass_gain
            );
        *bass_shelf.coefficients = *bass_shelf_coefficients;
    }
    if (low_mid_gain.isSmoothing())
    {
        float current_low_mid_gain = low_mid_gain.getNextValue();
        auto low_mid_peak_coefficients =
            juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                processSpec.sampleRate, low_mid_peak_frequency, low_mid_peak_q,
                current_low_mid_gain
            );
        *low_mid_peak.coefficients = *low_mid_peak_coefficients;
    }
    if (high_mid_gain.isSmoothing())
    {
        float current_high_mid_gain = high_mid_gain.getNextValue();
        auto high_mid_peak_coefficients =
            juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                processSpec.sampleRate, high_mid_peak_frequency,
                high_mid_peak_q, current_high_mid_gain
            );
        *high_mid_peak.coefficients = *high_mid_peak_coefficients;
    }
    if (treble_gain.isSmoothing())
    {
        float current_treble_gain = treble_gain.getNextValue();
        auto treble_shelf_coefficients =
            juce::dsp::IIR::Coefficients<float>::makeHighShelf(
                processSpec.sampleRate, treble_shelf_frequency, treble_shelf_q,
                current_treble_gain
            );
        *treble_shelf.coefficients = *treble_shelf_coefficients;
    }
}

void AmpEQ::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    float sampleRate = static_cast<float>(processSpec.sampleRate);
    setCoefficients();

    auto* channelData = buffer.getWritePointer(0);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float sample = channelData[i];
        applyEQ(sample, sampleRate);
        channelData[i] = sample;
    }
}

void AmpEQ::applyEQ(float& sample, float sampleRate)
{
    juce::ignoreUnused(sampleRate);
    sample = bass_shelf.processSample(sample);
    sample = low_mid_peak.processSample(sample);
    sample = high_mid_peak.processSample(sample);
    sample = treble_shelf.processSample(sample);
}

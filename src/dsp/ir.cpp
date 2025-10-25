#include "ir.h"
#include <algorithm>

#include "../assets/impulse_response_binary.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

void IRConvolver::resetSmoothedValues()
{
    float sample_rate = static_cast<float>(processSpec.sampleRate);
    sample_rate = std::max(1.0f, sample_rate);

    mix.reset(sample_rate, smoothing_time);
    mix.setCurrentAndTargetValue(raw_mix);
    level.reset(sample_rate, smoothing_time);
    level.setCurrentAndTargetValue(raw_level);
}

void IRConvolver::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
    resetSmoothedValues();
    loadIR();
}

void IRConvolver::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    if (type != loaded_type)
    {
        loadIR();
        loaded_type = type;
    }
    juce::ScopedNoDenormals noDenormals;
    juce::AudioBuffer<float> wetBuffer(
        buffer.getNumChannels(), buffer.getNumSamples()
    );
    juce::dsp::AudioBlock<float> wetBlock(wetBuffer);
    juce::dsp::ProcessContextNonReplacing<float> context(
        juce::dsp::AudioBlock<float>(buffer), wetBlock
    );
    convolution.process(context);

    // Only apply gain to the IR signal
    level.applyGain(wetBuffer, wetBuffer.getNumSamples());

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto* wetChannelData = wetBuffer.getReadPointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float current_mix = mix.getNextValue();
            channelData[sample] = (channelData[sample] * (1.0f - current_mix)) +
                                  (wetChannelData[sample] * current_mix);
        }
    }
}

void IRConvolver::loadIR()
{
    char* data;
    const int size = 3044;

    DBG("Loading IR type: " + juce::String(type));
    switch (type)
    {
    case 0:
        data = (char*)ImpulseResponseBinary::modern_410_wav;
        break;
    case 1:
        data = (char*)ImpulseResponseBinary::crunchy_212_wav;
        break;
    case 2:
        data = (char*)ImpulseResponseBinary::vintage_B15_wav;
        break;
    case 3:
        data = (char*)ImpulseResponseBinary::classic_810_wav;
        break;
    }

    convolution.loadImpulseResponse(
        data, size, juce::dsp::Convolution::Stereo::no,
        juce::dsp::Convolution::Trim::no, 0,
        juce::dsp::Convolution::Normalise::no
    );
    convolution.prepare(processSpec);
}

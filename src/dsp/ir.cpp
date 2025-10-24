#include "ir.h"

#include "../assets/impulse_response_binary.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

void IRConvolver::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
    loadIR();
}

void IRConvolver::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
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
    const char* data;
    const int size = 3044;

    switch (type)
    {
    case 0:
        data = (char*)ImpulseResponseBinary::modern_410_wav;
    case 1:
        data = (char*)ImpulseResponseBinary::crunchy_212_wav;
    case 2:
        data = (char*)ImpulseResponseBinary::vintage_B15_wav;
    case 3:
        data = (char*)ImpulseResponseBinary::classic_810_wav;
    }

    convolution.loadImpulseResponse(
        data, size, juce::dsp::Convolution::Stereo::yes,
        juce::dsp::Convolution::Trim::no, 0,
        juce::dsp::Convolution::Normalise::no
    );
    convolution.prepare(processSpec);
}

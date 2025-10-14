#include "ir.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

void IRConvolver::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
}

void IRConvolver::applyGain(juce::AudioBuffer<float>& buffer)
{
    if (juce::approximatelyEqual(gain, previousGain))
    {
        buffer.applyGain(gain);
    }
    else
    {
        buffer.applyGainRamp(0, buffer.getNumSamples(), previousGain, gain);
        previousGain = gain;
    }
}

void IRConvolver::process(juce::AudioBuffer<float>& buffer)
{
    if (bypass)
    {
        return;
    }
    if (!is_ir_loaded)
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
    applyGain(wetBuffer);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto* wetChannelData = wetBuffer.getReadPointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = (channelData[sample] * (1.0f - mix)) +
                                  (wetChannelData[sample] * mix);
        }
    }
}

void IRConvolver::loadIR()
{
    juce::File file = juce::File(filepath);

    if (!file.existsAsFile())
    {
        is_ir_loaded = false;
        DBG("File does not exist: " + filepath);
        return;
    }

    try
    {
        convolution.loadImpulseResponse(
            file, juce::dsp::Convolution::Stereo::yes,
            juce::dsp::Convolution::Trim::no, 0,
            juce::dsp::Convolution::Normalise::no
        );

        convolution.prepare(processSpec);
        DBG("Loaded IR from file: " + file.getFullPathName());
        is_ir_loaded = true;
    }
    catch (const std::exception& e)
    {
        DBG("Error loading IR: " + juce::String(e.what()));
        is_ir_loaded = false;
    }
    catch (...)
    {
        DBG("Unknown error loading IR.");
        is_ir_loaded = false;
    }
}

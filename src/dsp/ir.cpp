#include "ir.h"
#include <algorithm>

#include "../assets/impulse_response_binary.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

void IRConvolver::reset()
{
    convolution.reset();
    resetSmoothedValues();
    loadIR();
}

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
    reset();
}

void IRConvolver::process(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    if (type != loaded_type)
    {
        loadIR();
        loaded_type = type;
    }

    auto& block = context.getOutputBlock();
    const size_t num_channels = block.getNumChannels();
    const size_t num_samples = block.getNumSamples();
    dry_buffer.setSize((int)num_channels, (int)num_samples, false, false, true);
    juce::dsp::AudioBlock<float> dry_block(dry_buffer);
    dry_block.copyFrom(block);
    convolution.process(context);
    for (size_t ch = 0; ch < num_channels; ++ch)
    {
        auto* dry = dry_block.getChannelPointer(ch);
        auto* wet = block.getChannelPointer(ch);
        level.applyGain(wet, num_samples);

        for (size_t i = 0; i < num_samples; ++i)
        {
            float current_mix = mix.getNextValue();
            wet[i] = (wet[i] * current_mix) + (dry[i] * (1.0f - current_mix));
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

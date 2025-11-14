#include "ir.h"
#include <algorithm>

#include "../assets/ImpulseResponseBinaryMapping.h"
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
    const size_t num_channels = spec.numChannels;
    const size_t num_samples = spec.maximumBlockSize;
    dry_buffer.setSize((int)num_channels, (int)num_samples, false, false, true);
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
    juce::dsp::AudioBlock<float> dry_block(dry_buffer);
    dry_block.copyFrom(block);
    convolution.process(context);
    for (size_t ch = 0; ch < num_channels; ++ch)
    {
        auto* dry = dry_block.getChannelPointer(ch);
        auto* wet = block.getChannelPointer(ch);
        level.applyGain(wet, (int)num_samples);

        for (size_t i = 0; i < num_samples; ++i)
        {
            float current_mix = mix.getNextValue();
            wet[i] = (wet[i] * current_mix) + (dry[i] * (1.0f - current_mix));
        }
    }
}

void IRConvolver::loadIR()
{
    DBG("Loading IR type: " + juce::String(type));
    const int size = impulseResponseBinaryWavSizes[type];
    char* data = (char*)impulseResponseBinaryWavFiles[type];

    convolution.loadImpulseResponse(
        data, (size_t)size, juce::dsp::Convolution::Stereo::no,
        juce::dsp::Convolution::Trim::no, 0,
        juce::dsp::Convolution::Normalise::no
    );
    convolution.prepare(processSpec);
}

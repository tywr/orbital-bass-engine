#include "synth_voices.h"
#include <algorithm>

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

void SynthVoices::reset()
{
    raw_level.reset(processSpec.sampleRate, smoothing_time);
    raw_level.setCurrentAndTargetValue(raw_raw_level);
    square_level.reset(processSpec.sampleRate, smoothing_time);
    square_level.setCurrentAndTargetValue(raw_square_level);
    triangle_level.reset(processSpec.sampleRate, smoothing_time);
    triangle_level.setCurrentAndTargetValue(raw_triangle_level);
    octave_level.reset(processSpec.sampleRate, smoothing_time);
    octave_level.setCurrentAndTargetValue(raw_octave_level);
    master_level.reset(processSpec.sampleRate, smoothing_time);
    master_level.setCurrentAndTargetValue(raw_master_level);

    square_voice.reset();
    triangle_voice.reset();
    octave_voice.reset();
}

void SynthVoices::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;

    square_voice.prepare(spec);
    triangle_voice.prepare(spec);
    octave_voice.prepare(spec);

    square_buffer.setSize(
        (int)processSpec.numChannels, (int)processSpec.maximumBlockSize, false,
        false, true
    );
    triangle_buffer.setSize(
        (int)processSpec.numChannels, (int)processSpec.maximumBlockSize, false,
        false, true
    );
    octave_buffer.setSize(
        (int)processSpec.numChannels, (int)processSpec.maximumBlockSize, false,
        false, true
    );
    reset();
}

void SynthVoices::process(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    auto& block = context.getOutputBlock();
    const int num_samples = (int)block.getNumSamples();

    juce::dsp::AudioBlock<float> square_block(square_buffer);
    juce::dsp::AudioBlock<float> triangle_block(triangle_buffer);
    juce::dsp::AudioBlock<float> octave_block(octave_buffer);
    auto square_sub = square_block.getSubBlock(0, num_samples);
    auto triangle_sub = triangle_block.getSubBlock(0, num_samples);
    auto octave_sub = octave_block.getSubBlock(0, num_samples);
    square_sub.copyFrom(block);
    triangle_sub.copyFrom(block);
    octave_sub.copyFrom(block);

    auto square_context = juce::dsp::ProcessContextReplacing<float>(square_sub);
    auto triangle_context =
        juce::dsp::ProcessContextReplacing<float>(triangle_sub);
    auto octave_context = juce::dsp::ProcessContextReplacing<float>(octave_sub);

    square_voice.process(square_context);
    triangle_voice.process(triangle_context);
    octave_voice.process(octave_context);

    auto* ch = block.getChannelPointer(0);
    auto* tr = triangle_sub.getChannelPointer(0);
    auto* sq = square_sub.getChannelPointer(0);
    auto* oc = octave_sub.getChannelPointer(0);

    for (int i = 0; i < num_samples; ++i)
    {
        float square = sq[i] * square_level.getNextValue();
        float triangle = tr[i] * triangle_level.getNextValue();
        float octave = oc[i] * octave_level.getNextValue();
        float master = master_level.getNextValue();
        float current_raw_level = raw_level.getNextValue();

        ch[i] =
            master * (ch[i] * current_raw_level + square + triangle + octave);
    }
}

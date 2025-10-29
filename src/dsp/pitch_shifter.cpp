#include "pitch_shifter.h"
#include <algorithm>

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

void PitchShifter::reset()
{
    write_pos = 0;
    read_pos_a = crossfade_size;
    read_pos_b = crossfade_size * 1.5f;
    phase_a = 0.0f;
    phase_b = 0.5f;
}

void PitchShifter::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
    read_size =
        std::max((int)(spec.sampleRate * 0.05f), (int)spec.maximumBlockSize);
    crossfade_size = (int)(spec.sampleRate * crossfade_duration);
    read_buffer.setSize(1, read_size, false, true, true);
    read_buffer.clear();
    reset();
}

void PitchShifter::process(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    auto& block = context.getOutputBlock();
    const size_t num_samples = block.getNumSamples();

    auto* ch = block.getChannelPointer(0);
    for (size_t i = 0; i < num_samples; ++i)
    {
        float input = ch[i];
        int buffer_size = (int)read_buffer.getNumSamples();
        read_buffer.setSample(0, write_pos, input);

        float sample_a = readFromDelay(read_pos_a);
        float sample_b = readFromDelay(read_pos_b);

        float g_a =
            0.5f *
            (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * phase_a));
        float g_b =
            0.5f *
            (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * phase_b));

        float output = g_a * sample_a + g_b * sample_b;

        write_pos = (write_pos + 1) % buffer_size;
        read_pos_a += pitch_ratio;
        read_pos_b += pitch_ratio;

        phase_a += 1.0f / float(crossfade_size);
        phase_b += 1.0f / float(crossfade_size);

        ch[i] = output;
        if (phase_a >= 1.0f)
        {
            phase_a -= 1.0f;
            read_pos_a = write_pos - crossfade_size;
            if (read_pos_a < 0)
                read_pos_a += buffer_size;
        }
        if (phase_b >= 1.0f)
        {
            phase_b -= 1.0f;
            read_pos_b = write_pos - (0.5f * crossfade_size);
            if (read_pos_b < 0)
                read_pos_b += buffer_size;
        };
    }
}

float PitchShifter::readFromDelay(float pos)
{
    int i1 = ((int)pos) % read_buffer.getNumSamples();
    int i2 = (i1 + 1) % read_buffer.getNumSamples();
    float frac = pos - (int)pos;
    float s1 = read_buffer.getSample(0, i1);
    float s2 = read_buffer.getSample(0, i2);
    return s1 + frac * (s2 - s1);
}

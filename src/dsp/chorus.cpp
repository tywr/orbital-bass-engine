#include "chorus.h"
#include <algorithm>

#include "../assets/impulse_response_binary.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

void Chorus::reset()
{
    mix.reset(processSpec.sampleRate, smoothing_time);
    mix.setCurrentAndTargetValue(raw_mix);
    rate.reset(processSpec.sampleRate, smoothing_time);
    rate.setCurrentAndTargetValue(raw_rate);
    depth.reset(processSpec.sampleRate, smoothing_time);
    depth.setCurrentAndTargetValue(raw_depth);
}

void Chorus::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
    delay_buffer.setSize(2, (int)(max_delay_time * 2), false, false, true);

    lfo_right.prepare(spec);
    lfo_right.initialise([](float x) { return std::sin(x); });
    lfo_right.setFrequency(raw_rate);

    lfo_left.prepare(spec);
    lfo_left.initialise([](float x) { return std::cos(x); });
    lfo_left.setFrequency(raw_rate);

    reset();
}

void Chorus::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& block = context.getOutputBlock();
    const size_t num_samples = block.getNumSamples();
    float sample_rate = (float)processSpec.sampleRate;

    auto* left = block.getChannelPointer(0);
    auto* right = block.getChannelPointer(1);

    auto* delay_left = delay_buffer.getWritePointer(0);
    auto* delay_right = delay_buffer.getWritePointer(1);

    size_t delay_buf_size = (size_t)delay_buffer.getNumSamples();
    size_t& wl = write_position_left;
    size_t& wr = write_position_right;

    for (size_t i = 0; i < num_samples; ++i)
    {
        float current_mix = mix.getNextValue();
        float current_depth = depth.getNextValue();

        float rval = lfo_right.processSample(0.0f);
        float lval = lfo_left.processSample(0.0f);

        float rdelay =
            ((current_depth / 1000.0f) * sample_rate) * (rval + 1.0f) * 0.5f;
        float ldelay =
            ((current_depth / 1000.0f) * sample_rate) * (lval + 1.0f) * 0.5f;

        float read_pos_left = (float)wl - ldelay;
        float read_pos_right = (float)wr - rdelay;

        if (read_pos_left < 0)
            read_pos_left += (float)delay_buf_size;
        if (read_pos_right < 0)
            read_pos_right += (float)delay_buf_size;

        size_t index_left_1 = (size_t)read_pos_left;
        size_t index_left_2 = (index_left_1 + 1) % delay_buf_size;
        size_t index_right_1 = (size_t)read_pos_right;
        size_t index_right_2 = (index_right_1 + 1) % delay_buf_size;

        float frac_left = read_pos_left - (float)index_left_1;
        float frac_right = read_pos_right - (float)index_right_1;

        float delayed_left = delay_left[index_left_1] * (1.0f - frac_left) +
                             delay_left[index_left_2] * frac_left;
        float delayed_right = delay_right[index_right_1] * (1.0f - frac_right) +
                              delay_right[index_right_2] * frac_right;

        delay_left[wl] = left[i];
        delay_right[wr] = right[i];

        // Careful, we handle mono as input, so we copy left to right !
        left[i] =
            (left[i] * current_mix) + (delayed_left * (1.0f - current_mix));
        right[i] =
            (left[i] * current_mix) + (delayed_right * (1.0f - current_mix));

        wl = (wl + 1) % delay_buf_size;
        wr = (wr + 1) % delay_buf_size;
    }
}

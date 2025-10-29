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
    write_position = 0;
}

void Chorus::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;

    pre_hpf.prepare(spec);
    auto hpf_coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        spec.sampleRate, pre_hpf_cutoff, 0.707f
    );
    *pre_hpf.coefficients = *hpf_coefficients;

    pre_lpf.prepare(spec);
    auto lpf_coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        spec.sampleRate, pre_lpf_cutoff, 0.707f
    );
    *pre_lpf.coefficients = *lpf_coefficients;

    lfo_right.prepare(spec);
    lfo_right.initialise([](float x) { return std::sin(x); });
    lfo_right.setFrequency(raw_rate);

    lfo_left.prepare(spec);
    lfo_left.initialise(
        [](float x) { return std::sin(x + juce::MathConstants<float>::pi); }
    );
    lfo_left.setFrequency(raw_rate);

    delay_line.prepare(spec);
    delay_line.setMaximumDelayInSamples((int)(max_delay_time *
                                              spec.sampleRate));
    reset();
}

void Chorus::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    auto& block = context.getOutputBlock();
    const size_t num_samples = block.getNumSamples();
    float sample_rate = (float)processSpec.sampleRate;

    auto* left = block.getChannelPointer(0);
    auto* right = block.getChannelPointer(1);

    for (size_t i = 0; i < num_samples; ++i)
    {
        float current_mix = mix.getNextValue();
        float current_depth = depth.getNextValue();

        float input_sample =
            pre_lpf.processSample(pre_hpf.processSample(left[i]));

        float lval = lfo_left.processSample(0.0f);
        float rval = lfo_right.processSample(0.0f);

        float ldelay = base_delay_time + current_depth * sample_rate * lval;
        float rdelay = base_delay_time + current_depth * sample_rate * rval;

        float lvalue = delay_line.popSample(0, rdelay * sample_rate, true);
        float rvalue = delay_line.popSample(1, ldelay * sample_rate, true);

        delay_line.pushSample(0, input_sample);
        delay_line.pushSample(1, input_sample);

        left[i] = (input_sample * (1.0f - current_mix) + lvalue * current_mix);
        right[i] = (input_sample * (1.0f - current_mix) + rvalue * current_mix);
    }
}

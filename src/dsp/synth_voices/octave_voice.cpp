#include "octave_voice.h"
#include <cmath>

#include <juce_dsp/juce_dsp.h>

void OctaveVoice::prepare(const juce::dsp::ProcessSpec& spec)
{
    process_spec = spec;
    od_spec = spec;
    od_spec.sampleRate *= 2;

    oversampler.reset();
    oversampler.initProcessing(static_cast<size_t>(od_spec.maximumBlockSize));

    noise_gate.prepare(spec);
    noise_gate.setThreshold(-50.0f);

    pre_lpf.prepare(spec);
    auto pre_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            spec.sampleRate, pre_lpf_cutoff
        );
    *pre_lpf.coefficients = *pre_lpf_coefficients;

    post_lpf.prepare(od_spec);
    auto post_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            spec.sampleRate, post_lpf_cutoff
        );
    *post_lpf.coefficients = *post_lpf_coefficients;
}

void OctaveVoice::reset()
{
    sub_state = -1;
    cross_count = 0;
    prev_envelope = 0.0f;
}

void OctaveVoice::process(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    pre_lpf.process(context);
    noise_gate.process(context);

    auto& block = context.getOutputBlock();

    auto os_block = oversampler.processSamplesUp(block);
    juce::dsp::ProcessContextReplacing<float> os_context(os_block);

    int num_samples = os_block.getNumSamples();
    auto* ch = os_block.getChannelPointer(0);

    for (int i = 0; i < num_samples; ++i)
    {
        float sample = std::abs(ch[i]);
        float envelope =
            0.9f * prev_envelope + 0.1f * juce::jmin(sample / threshold, 1.0f);
        int state = (sample >= threshold) ? 1 : -1;
        if (prev_state != state)
        {
            cross_count++;

            if (cross_count >= 2)
            {
                sub_state = -sub_state;
                cross_count = 0;
            }
        }
        ch[i] = envelope * post_lpf.processSample(sub_state);
        prev_state = state;
        prev_envelope = envelope;
    }
    oversampler.processSamplesDown(block);
}

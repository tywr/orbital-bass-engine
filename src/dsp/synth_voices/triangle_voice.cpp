#include "triangle_voice.h"
#include <cmath>

#include <juce_dsp/juce_dsp.h>

void TriangleVoice::prepare(const juce::dsp::ProcessSpec& spec)
{
    process_spec = spec;
    oversampled_spec = spec;
    oversampled_spec.sampleRate *= 2;

    oversampler.reset();
    oversampler.initProcessing(
        static_cast<size_t>(oversampled_spec.maximumBlockSize)
    );

    // prior to oversampling
    pre_lpf.prepare(process_spec);
    auto triangle_pre_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, pre_lpf_cutoff
        );
    *pre_lpf.coefficients = *triangle_pre_lpf_coefficients;

    pre_hpf.prepare(process_spec);
    auto triangle_pre_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            process_spec.sampleRate, pre_hpf_cutoff
        );
    *pre_hpf.coefficients = *triangle_pre_hpf_coefficients;

    // with oversampling specs
    noise_gate.prepare(oversampled_spec);
    noise_gate.setThreshold(-50.0f);

    post_lpf.prepare(oversampled_spec);
    auto post_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            process_spec.sampleRate, post_lpf_cutoff
        );
    *post_lpf.coefficients = *post_lpf_coefficients;

    post_hpf.prepare(oversampled_spec);
    auto post_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            process_spec.sampleRate, post_hpf_cutoff
        );
    *post_hpf.coefficients = *post_hpf_coefficients;
    reset();
}

void TriangleVoice::reset()
{
    oversampler.reset();
}

void TriangleVoice::process(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    pre_hpf.process(context);
    pre_lpf.process(context);

    auto& block = context.getOutputBlock();
    auto os_block = oversampler.processSamplesUp(block);
    juce::dsp::ProcessContextReplacing<float> os_context(os_block);
    noise_gate.process(os_context);
    square_waveshaper.process(os_context);

    int num_samples = (int)os_block.getNumSamples();
    auto* ch = os_block.getChannelPointer(0);
    for (int i = 0; i < num_samples; ++i)
    {
        triangle_signal = triangle_signal * 0.99f + ch[i] * triangle_gain;
        ch[i] = triangle_signal;
    }

    post_hpf.process(os_context);
    post_lpf.process(os_context);
    oversampler.processSamplesDown(block);
}

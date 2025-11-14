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

    envelope_lpf.prepare(spec);
    auto envelope_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            spec.sampleRate, envelope_lpf_cutoff
        );
    *envelope_lpf.coefficients = *envelope_lpf_coefficients;

    pre_lpf.prepare(spec);
    pre_lpf_2.prepare(spec);
    pre_lpf_3.prepare(spec);
    auto pre_lpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(
            spec.sampleRate, pre_lpf_cutoff
        );
    *pre_lpf.coefficients = *pre_lpf_coefficients;
    *pre_lpf_2.coefficients = *pre_lpf_coefficients;
    *pre_lpf_3.coefficients = *pre_lpf_coefficients;

    pre_hpf.prepare(spec);
    auto pre_hpf_coefficients =
        juce::dsp::IIR::Coefficients<float>::makeHighPass(
            spec.sampleRate, pre_hpf_cutoff
        );
    *pre_hpf.coefficients = *pre_hpf_coefficients;

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
    // envelope = 0.0f;
}

void OctaveVoice::process(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    pre_lpf.process(context);
    pre_lpf_2.process(context);
    pre_lpf_3.process(context);
    pre_hpf.process(context);
    noise_gate.process(context);

    auto& block = context.getOutputBlock();

    auto os_block = oversampler.processSamplesUp(block);
    juce::dsp::ProcessContextReplacing<float> os_context(os_block);

    int num_samples = (int)os_block.getNumSamples();
    auto* ch = os_block.getChannelPointer(0);

    for (int i = 0; i < num_samples; ++i)
    {
        float sample = ch[i];
        float envelope =
            envelope_lpf.processSample(std::tanh(sample * std::tanh(sample)));

        int state = prev_state;
        if (sample > +threshold)
            state = +1;
        else if (sample < -threshold)
            state = -1;

        if (prev_state != state)
        {
            cross_count++;
            if (cross_count >= 2)
            {
                sub_state = -sub_state;
                cross_count = 0;
            }
        }
        ch[i] = 16.0f * envelope * post_lpf.processSample(sub_state);
        prev_state = state;
    }
    oversampler.processSamplesDown(block);
}

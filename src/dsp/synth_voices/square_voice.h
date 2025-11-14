#pragma once

#include <cmath>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class SquareVoice : juce::dsp::ProcessorBase
{
  public:
    void process(
        const juce::dsp::ProcessContextReplacing<float>& context
    ) override;
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;
    void prepareFilters();

  private:
    juce::dsp::ProcessSpec process_spec{44100.0f, 512, 2};
    juce::dsp::ProcessSpec oversampled_spec{44100.0f, 512, 2};

    juce::dsp::IIR::Filter<float> pre_hpf;
    float pre_hpf_cutoff = 30.0f;

    juce::dsp::IIR::Filter<float> pre_lpf;
    float pre_lpf_cutoff = 330.0f;

    juce::dsp::IIR::Filter<float> post_lpf;
    float post_lpf_cutoff = 330.0f;

    float prev_envelope = 0.0f;
    juce::dsp::WaveShaper<float> square_waveshaper{
        [](float x) { return std::tanh(x / 0.005f); }
    };
    juce::dsp::NoiseGate<float> noise_gate = juce::dsp::NoiseGate<float>();

    juce::dsp::Oversampling<float> oversampler{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
};

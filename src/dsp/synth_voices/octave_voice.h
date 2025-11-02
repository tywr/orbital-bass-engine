#pragma once

#include <cmath>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class OctaveVoice : juce::dsp::ProcessorBase
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
    juce::dsp::ProcessSpec od_spec{44100.0f, 512, 2};

    int sub_state = -1;
    int cross_count = 0;
    int prev_state = -1;
    // float envelope = 0.0f;
    int peak = 0;
    float threshold = 0.01f;
    bool below_threshold = true;

    juce::dsp::NoiseGate<float> noise_gate = juce::dsp::NoiseGate<float>();

    juce::dsp::IIR::Filter<float> envelope_lpf;
    float envelope_lpf_cutoff = 0.443f / 0.001f;

    juce::dsp::IIR::Filter<float> pre_lpf;
    juce::dsp::IIR::Filter<float> pre_lpf_2;
    juce::dsp::IIR::Filter<float> pre_lpf_3;
    float pre_lpf_cutoff = 330.0f;

    juce::dsp::IIR::Filter<float> pre_hpf;
    float pre_hpf_cutoff = 30.0f;

    juce::dsp::IIR::Filter<float> post_lpf;
    float post_lpf_cutoff = 330.0f;

    juce::dsp::Oversampling<float> oversampler{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
};

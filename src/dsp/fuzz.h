#pragma once

#include "circuits/germanium_diode.h"
#include <cmath>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class Fuzz : juce::dsp::ProcessorBase
{
  public:
    void process(
        const juce::dsp::ProcessContextReplacing<float>& context
    ) override;
    void processSquare(
        const juce::dsp::ProcessContextReplacing<float>& context
    );
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;
    void resetSmoothedValues();
    void resetFilters();
    void updateToneFilter();
    void updateDriveFilters();
    void applyOverdrive(float& sample);
    void prepareFilters();
    void setTone(float new_tone)
    {
        float v = juce::jlimit(0.0f, 10.0f, new_tone);
        tone.setTargetValue(v);
        raw_tone = new_tone;
    }
    void setSustain(float new_sustain)
    {
        float v = juce::jlimit(0.0f, 10.0f, new_sustain);
        sustain.setTargetValue(v);
        raw_sustain = new_sustain;
    }
    void setMix(float new_mix)
    {
        float v = juce::jlimit(0.0f, 1.0f, new_mix);
        mix.setTargetValue(v);
        raw_mix = new_mix;
    }
    void setLevel(float new_level)
    {
        float v = juce::jlimit(0.0f, 3.0f, new_level);
        level.setTargetValue(v);
        raw_level = new_level;
    }

  private:
    juce::dsp::ProcessSpec process_spec{44100.0f, 512, 2};
    float smoothing_time = 0.05f;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> tone, sustain,
        mix, level;
    float raw_tone = 1.0f;
    float raw_sustain = 1.0f;
    float raw_mix = 1.0f;
    float raw_level = 1.0f;

    juce::AudioBuffer<float> square_buffer;

    juce::dsp::IIR::Filter<float> square_pre_hpf;
    float pre_hpf_cutoff = 25.0f;

    juce::dsp::IIR::Filter<float> square_pre_lpf;
    float pre_lpf_cutoff = 1540.0f;

    juce::dsp::IIR::Filter<float> tone_filter;
    juce::dsp::IIR::Filter<float> drive_filter_1;
    juce::dsp::IIR::Filter<float> drive_filter_2;

    juce::dsp::IIR::Filter<float> square_post_filter_1;
    float square_post_lpf_cutoff_1 = 3000.0f;
    float square_post_lpf_q_1 = 0.67f;

    juce::dsp::IIR::Filter<float> square_post_filter_2;
    float square_post_lpf_cutoff_2 = 3000.0f;
    float square_post_lpf_q_2 = 0.67f;

    juce::dsp::IIR::Filter<float> square_post_filter_3;
    float square_post_lpf_cutoff_3 = 3000.0f;
    float square_post_lpf_q_3 = 0.67f;

    juce::dsp::WaveShaper<float> diode_pair{[](float x)
                                            { return std::tanh(3 * x); }};
    juce::dsp::NoiseGate<float> noise_gate = juce::dsp::NoiseGate<float>();

    juce::dsp::Oversampling<float> oversampler{
        2, 4,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
};

#pragma once

#include "../circuits/cmos.h"
#include "../circuits/silicon_diode.h"
#include "overdrive.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class HeliosOverdrive : public Overdrive
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    float driveToGain(float) override;
    float charToFreq(float);
    void applyOverdrive(float& sample, float sampleRate) override;

  private:
    juce::dsp::IIR::Filter<float> pre_hpf;
    float pre_hpf_cutoff = 50.0f;

    juce::dsp::IIR::Filter<float> pre_lpf;
    // float pre_lpf_cutoff = 1540.0f;
    float pre_lpf_cutoff = 7400.0f;
    float pre_lpf_q = 0.5f;

    juce::dsp::IIR::Filter<float> lowmids_lpf;
    // float lowmids_lpf_cutoff = 330.0f;
    float lowmids_lpf_cutoff = 250.0f;

    juce::dsp::IIR::Filter<float> mid_hpf;
    // float mid_hpf_cutoff = 219.0f;
    float mid_hpf_cutoff = 219.0f;

    juce::dsp::IIR::Filter<float> era_lpf;
    float era_lpf_cutoff = 1.0f;
    float era_lpf_q = 0.5f;

    juce::dsp::IIR::Filter<float> dc_hpf;
    float dc_hpf_cutoff = 20.0f;

    juce::dsp::IIR::Filter<float> dc_hpf2;
    float dc_hpf2_cutoff = 20.0f;

    juce::dsp::IIR::Filter<float> post_lpf;
    float post_lpf_cutoff = 500.0f;
    float post_lpf_q = 0.1f;

    juce::dsp::IIR::Filter<float> post_lpf2;
    float post_lpf2_cutoff = 7400.0f;

    // triode parameters

    // float padding = juce::Decibels::decibelsToGain(-16.0f);
    float padding = juce::Decibels::decibelsToGain(12.0f);

    CMOS cmos = CMOS();
    SiliconDiode diode_plus = SiliconDiode(44100.0f, true);
    SiliconDiode diode_minus = SiliconDiode(44100.0f, false);

    juce::dsp::Oversampling<float> oversampler2x{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
};

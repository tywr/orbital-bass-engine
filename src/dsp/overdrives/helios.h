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
    void updateEraFilter(float sampleRate);
    void applyOverdrive(float& sample, float sampleRate) override;

  private:
    float era;

    juce::dsp::IIR::Filter<float> pre_hpf;
    float pre_hpf_cutoff = 50.0f;

    juce::dsp::IIR::Filter<float> pre_lpf;
    float pre_lpf_cutoff = 3300.0f;
    float pre_lpf_q = 0.5f;

    juce::dsp::IIR::Filter<float> lowmids_lpf;
    float lowmids_lpf_cutoff = 330.0f;

    juce::dsp::IIR::Filter<float> mid_hpf;
    float mid_hpf_cutoff = 219.0f;

    juce::dsp::IIR::Filter<float> era_mid_scoop;
    float era_mid_scoop_frequency = 1200.0f;
    float era_mid_scoop_q = 0.5f;
    float era_mid_scoop_gain = juce::Decibels::decibelsToGain(-4.0f);

    juce::dsp::IIR::Filter<float> era_shelf;
    float era_shelf_cutoff = 500.0f;
    float era_shelf_q = 0.7f;
    float era_shelf_gain = juce::Decibels::decibelsToGain(-18.0f);

    juce::dsp::IIR::Filter<float> dc_hpf;
    float dc_hpf_cutoff = 20.0f;

    juce::dsp::IIR::Filter<float> post_lpf;
    float post_lpf_cutoff = 3300.0f;
    float post_lpf_q = 0.5f;

    juce::dsp::IIR::Filter<float> post_lpf2;
    float post_lpf2_cutoff = 3300.0f;

    juce::dsp::IIR::Filter<float> post_lpf3;
    float post_lpf3_cutoff = 7200.0f;

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

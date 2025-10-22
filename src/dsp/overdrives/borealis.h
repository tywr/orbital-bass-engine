#pragma once

#include "../circuits/cmos.h"
#include "../circuits/germanium_diode.h"
#include "overdrive.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class BorealisOverdrive : public Overdrive
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    void resetSmoothedValues();
    void resetFilters();
    float driveToGain(float);
    void applyOverdrive(float& sample);
    void prepareFilters();
    void updateXFilter();
    void updateLowFilter();

  private:
    float x_output_padding = juce::Decibels::decibelsToGain(-20.0f);

    juce::dsp::IIR::Filter<float> pre_hpf;
    float pre_hpf_cutoff = 50.0f;

    juce::dsp::IIR::Filter<float> pre_lpf;
    float pre_lpf_cutoff = 3300.0f;

    juce::dsp::IIR::Filter<float> lowmids_lpf;
    float lowmids_lpf_cutoff = 330.0f;

    juce::dsp::IIR::Filter<float> x_hpf;
    juce::dsp::IIR::Filter<float> bass_lpf;

    juce::dsp::IIR::Filter<float> post_lpf;
    float post_lpf_cutoff = 3300.0f;

    juce::dsp::IIR::Filter<float> post_lpf2;
    float post_lpf2_cutoff = 3300.0f;

    juce::dsp::IIR::Filter<float> post_lpf3;
    float post_lpf3_cutoff = 7200.0f;

    GermaniumDiode diode = GermaniumDiode(44100.0f);
    CMOS cmos = CMOS();

    juce::dsp::Oversampling<float> oversampler2x{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
};

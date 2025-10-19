#pragma once

#include "../circuits/cmos.h"
#include "overdrive.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class HeliosOverdrive : public Overdrive
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    void resetSmoothedValues();
    void resetFilters();
    float driveToGain(float);
    void updateAttackFilter();
    void updateGruntFilter();
    void updateMidScoop();
    void updateDriveFilter();
    void applyOverdrive(float& sample);
    void prepareFilters();

  private:
    juce::dsp::IIR::Filter<float> pre_hpf;
    float pre_hpf_cutoff = 50.0f;

    juce::dsp::IIR::Filter<float> pre_lpf;
    float pre_lpf_cutoff = 1540.0f;

    juce::dsp::IIR::Filter<float> drive_filter;
    juce::dsp::IIR::Filter<float> attack_shelf;

    juce::dsp::IIR::Filter<float> post_lpf;
    float post_lpf_cutoff = 2200.0f;

    juce::dsp::IIR::Filter<float> post_lpf2;
    float post_lpf2_cutoff = 7240.0f;

    juce::dsp::IIR::Filter<float> post_lpf3;
    float post_lpf3_cutoff = 1540.0f;

    CMOS cmos = CMOS();

    juce::dsp::Oversampling<float> oversampler2x{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
};

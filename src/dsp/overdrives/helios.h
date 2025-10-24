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
    void updateEraFilter();
    void updateMidScoop();
    void updateDriveFilter();
    void applyOverdrive(float& sample);
    void prepareFilters();

  private:
    juce::dsp::IIR::Filter<float> pre_hpf;
    float pre_hpf_cutoff = 50.0f;

    juce::dsp::IIR::Filter<float> pre_lpf;
    float pre_lpf_cutoff = 1540.0f;

    juce::dsp::IIR::Filter<float> era_filter;
    juce::dsp::IIR::Filter<float> vmt_drive_filter;
    juce::dsp::IIR::Filter<float> b3k_drive_filter;
    juce::dsp::IIR::Filter<float> vmt_attack_shelf;
    juce::dsp::IIR::Filter<float> b3k_attack_shelf;

    juce::dsp::IIR::Filter<float> b3k_pre_filter_1;
    juce::dsp::IIR::Filter<float> b3k_pre_filter_2;
    juce::dsp::IIR::Filter<float> vmt_pre_filter;

    juce::dsp::IIR::Filter<float> b3k_post_filter_1;

    juce::dsp::IIR::Filter<float> b3k_post_filter_2;
    float b3k_post_lpf_cutoff_2 = 4877.0f;
    float b3k_post_lpf_q_2 = 1.0f;

    juce::dsp::IIR::Filter<float> b3k_post_filter_3;
    float b3k_post_lpf_cutoff_3 = 3337.0f;
    float b3k_post_lpf_q_3 = 0.67f;

    juce::dsp::IIR::Filter<float> vmt_post_filter_1;

    juce::dsp::IIR::Filter<float> vmt_post_filter_2;
    float vmt_post_lpf_cutoff_2 = 10730.0f;
    float vmt_post_lpf_q_2 = 1.0f;

    juce::dsp::IIR::Filter<float> vmt_post_filter_3;
    float vmt_post_lpf_cutoff_3 = 2287.0f;
    float vmt_post_lpf_q_3 = 0.67f;

    CMOS cmos = CMOS();
    CMOS cmos2 = CMOS();

    juce::dsp::Oversampling<float> oversampler2x{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
};

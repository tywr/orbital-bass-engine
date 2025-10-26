#pragma once

#include "../circuits/cmos.h"
#include "overdrive.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class BorealisOverdrive : public Overdrive
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(
        const juce::dsp::ProcessContextReplacing<float>& context
    ) override;
    void reset() override;
    void resetSmoothedValues();
    void resetFilters();
    float driveToGain(float);
    void prepareFilters();
    void updateXFilter();
    void updateDriveFilter();
    void updateLowFilter();

  private:
    juce::AudioBuffer<float> high_buffer;
    juce::AudioBuffer<float> low_buffer;

    float x_output_padding = juce::Decibels::decibelsToGain(-20.0f);

    juce::dsp::IIR::Filter<float> drive_filter;

    juce::dsp::IIR::Filter<float> pre_hpf;
    float pre_hpf_cutoff = 50.0f;

    juce::dsp::IIR::Filter<float> pre_lpf;
    float pre_lpf_cutoff = 1590.0f;

    juce::dsp::IIR::Filter<float> lowmids_lpf;
    float lowmids_lpf_cutoff = 330.0f;

    juce::dsp::IIR::Filter<float> x_hpf;
    juce::dsp::IIR::Filter<float> bass_lpf;

    juce::dsp::IIR::Filter<float> post_lpf;
    float post_lpf_cutoff = 4877.0f;
    float post_lpf_q = 1.0f;

    juce::dsp::IIR::Filter<float> post_lpf2;
    float post_lpf2_cutoff = 3337.0f;
    float post_lpf2_q = 0.67f;

    CMOS cmos = CMOS();

    juce::dsp::Oversampling<float> oversampler2x{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
};

#pragma once

#include "../circuits/opamp.h"
#include "overdrive.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class NebulaOverdrive : public Overdrive
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    float driveToGain(float);
    void applyOverdrive(float& sample);
    void resetSmoothedValues();
    void updateDriveFilter();
    void prepareFilters();

  private:
    OpAmp opamp = OpAmp();

    juce::dsp::IIR::Filter<float> pre_hpf;
    float pre_hpf_cutoff = 50.0f;

    juce::dsp::IIR::Filter<float> drive_filter;

    juce::dsp::IIR::Filter<float> alpha_1_filter;
    float alpha_1_gain = juce::Decibels::decibelsToGain(-3.5f);
    float alpha_1_frequency = 666.0f;

    juce::dsp::IIR::Filter<float> alpha_2_filter;
    float alpha_2_frequency = 177.0f;

    juce::dsp::IIR::Filter<float> alpha_3_filter;
    float alpha_3_frequency = 177.0f;

    juce::dsp::IIR::Filter<float> omega_1_filter;
    float omega_1_frequency = 106.0f;

    juce::dsp::IIR::Filter<float> omega_2_filter;
    float omega_2_frequency = 3300.0f;

    juce::dsp::IIR::Filter<float> omega_3_filter;
    float omega_3_frequency = 300.0f;
    float omega_3_gain = juce::Decibels::decibelsToGain(-15.0f);

    juce::dsp::Oversampling<float> oversampler2x{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
};

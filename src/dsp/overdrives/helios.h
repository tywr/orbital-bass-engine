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
    void reset() override;
    void resetSmoothedValues();
    void resetFilters();
    float driveToGain(float);
    void updateAttackFilter();
    void updateGruntFilter();
    void updateMidScoop();
    void applyOverdrive(float& sample, float drive_gain);
    void prepareFilters();

  private:
    juce::dsp::IIR::Filter<float> pre_hpf;
    float pre_hpf_cutoff = 50.0f;

    juce::dsp::IIR::Filter<float> pre_lpf;
    float pre_lpf_cutoff = 1540.0f;
    float pre_lpf_q = 0.5f;

    juce::dsp::IIR::Filter<float> lowmids_lpf;
    float lowmids_lpf_cutoff = 330.0f;

    juce::dsp::IIR::Filter<float> mid_hpf;
    float mid_hpf_cutoff = 219.0f;

    juce::dsp::IIR::Filter<float> mid_scoop;

    juce::dsp::IIR::Filter<float> grunt_filter;

    juce::dsp::IIR::Filter<float> attack_shelf;

    juce::dsp::IIR::Filter<float> dc_hpf;
    float dc_hpf_cutoff = 20.0f;

    juce::dsp::IIR::Filter<float> post_lpf;
    float post_lpf_cutoff = 7234.0f;

    juce::dsp::IIR::Filter<float> post_lpf2;
    float post_lpf2_cutoff = 1540.0f;

    juce::dsp::IIR::Filter<float> post_lpf3;
    float post_lpf3_cutoff = 3300.0f;

    CMOS cmos = CMOS();
    SiliconDiode diode_plus = SiliconDiode(44100.0f, true);
    SiliconDiode diode_minus = SiliconDiode(44100.0f, false);

    juce::dsp::Oversampling<float> oversampler2x{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
};

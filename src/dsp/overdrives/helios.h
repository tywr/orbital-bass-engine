#pragma once

#include "../circuits/cmos.h"
#include "../circuits/cmos_approx.h"
#include "overdrive.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class HeliosOverdrive : public Overdrive
{
  public:
    void process(
        const juce::dsp::ProcessContextReplacing<float>& context
    ) override;
    void processVMT(const juce::dsp::ProcessContextReplacing<float>& context);
    void processB3K(const juce::dsp::ProcessContextReplacing<float>& context);
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void reset() override;
    void resetSmoothedValues();
    void resetFilters();
    float driveToGain(float);
    void updateAttackFilter();
    void updateGruntFilter();
    void updateEraFilter();
    void updateDriveFilter();
    void applyOverdrive(float& sample);
    void prepareFilters();

  private:
    juce::AudioBuffer<float> vmt_buffer;

    juce::dsp::IIR::Filter<float> vmt_pre_hpf;
    float pre_hpf_cutoff = 70.0f;

    juce::dsp::IIR::Filter<float> vmt_pre_lpf;
    float pre_lpf_cutoff = 1540.0f;
    float pre_lpf_q = 1.5f;

    juce::dsp::IIR::Filter<float> era_filter;
    juce::dsp::IIR::Filter<float> vmt_drive_filter;
    juce::dsp::IIR::Filter<float> vmt_attack_shelf;
    juce::dsp::IIR::Filter<float> vmt_grunt_filter;
    juce::dsp::IIR::Filter<float> vmt_era_filter;

    juce::dsp::IIR::Filter<float> vmt_pre_filter;
    juce::dsp::IIR::Filter<float> vmt_pre_filter_2;
    juce::dsp::IIR::Filter<float> vmt_pre_filter_3;

    juce::dsp::IIR::Filter<float> vmt_post_filter_2;
    float vmt_post_lpf_cutoff_2 = 10730.0f;
    float vmt_post_lpf_q_2 = 1.0f;

    juce::dsp::IIR::Filter<float> vmt_post_filter_3;
    float vmt_post_lpf_cutoff_3 = 2287.0f;
    float vmt_post_lpf_q_3 = 0.67f;

    CMOS cmos = CMOS();
    juce::dsp::WaveShaper<float> mu_amp{
        [](float x) { return 4.0f * (x + 0.04f * std::tanh(0.1f + x)); }
    };

    juce::dsp::Oversampling<float> oversampler2x{
        2, 1,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
    juce::dsp::Oversampling<float> oversampler4x{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
    juce::dsp::Oversampling<float> oversampler8x{
        2, 3,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
    std::vector<juce::dsp::Oversampling<float>*> oversamplers = {
        &oversampler2x, &oversampler4x, &oversampler8x
    };
    size_t oversampling_index = 0;
};

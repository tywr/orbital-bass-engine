#pragma once

#include "../circuits/bjt.h"
#include "../circuits/germanium_diode.h"
#include "overdrive.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class BorealisOverdrive : public Overdrive
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void setCoefficients();
    void updateAttackFilter();
    float charToGain(float);
    float driveToGain(float) override;
    float driveToFrequency(float);
    void applyOverdrive(float& sample, float sampleRate);

  private:
    float current_attack;

    juce::dsp::IIR::Filter<float> ff1_hpf;
    float ff1_hpf_cutoff = 50.0f;

    juce::dsp::IIR::Filter<float> ff1_lpf;
    float ff1_lpf_cutoff = 330.0f;

    juce::dsp::IIR::Filter<float> attack_shelf;
    float attack_shelf_freq = 500.0f;

    juce::dsp::IIR::Filter<float> pre_hpf;
    float pre_hpf_cutoff = 500.0f;

    juce::dsp::IIR::Filter<float> pre_lpf;
    float pre_lpf_cutoff = 3300.0f;

    juce::dsp::IIR::Filter<float> post_lpf;
    float post_lpf_cutoff = 3400.0f;

    float padding = juce::Decibels::decibelsToGain(-6.0f);

    GermaniumDiode diode = GermaniumDiode(44100.0f);

    juce::dsp::Oversampling<float> oversampler2x{
        2, 2,
        juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
        true, false
    };
};

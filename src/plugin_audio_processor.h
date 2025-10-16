#pragma once

#include "dsp/amp_eq.h"
#include "dsp/compressor.h"
#include "dsp/ir.h"
#include "dsp/overdrives/borealis.h"
#include "dsp/overdrives/helios.h"
#include "dsp/overdrives/nebula.h"
#include "dsp/overdrives/overdrive.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
class PluginAudioProcessor final
    : public juce::AudioProcessor,
      public juce::AudioProcessorValueTreeState::Listener
{
  public:
    PluginAudioProcessor();
    ~PluginAudioProcessor() override;

    void parameterChanged(
        const juce::String& parameterID, float newValue
    ) override;
    void setParameterValue(juce::String, float);
    void setupParameterHandlers();
    void prepareParameters();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    // Decay factor for level smoothing
    double decayFactor = 0.95f;
    juce::Value inputLevel;                // in dB
    juce::Value outputLevel;               // in dB
    juce::Value compressorGainReductionDb; // in dB
    void updateInputLevel(juce::AudioBuffer<float>& buffer);
    void updateOutputLevel(juce::AudioBuffer<float>& buffer);
    void applyGain(std::atomic<float>*, float&, juce::AudioBuffer<float>&);

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

  private:
    juce::AudioProcessorValueTreeState parameters;
    juce::AudioProcessorValueTreeState::ParameterLayout parameterLayout;
    using ParamHandler = std::function<void(float)>;
    std::unordered_map<std::string, ParamHandler> parameterHandlers;

    Compressor compressor;

    std::atomic<Overdrive*> current_overdrive = nullptr;
    HeliosOverdrive helios_overdrive;
    BorealisOverdrive borealis_overdrive;
    NebulaOverdrive nebula_overdrive;
    std::vector<Overdrive*> overdrives = {
        &helios_overdrive, &borealis_overdrive, &nebula_overdrive
    };

    AmpEQ amp_eq;

    IRConvolver irConvolver;

    float smoothing_time = 0.05f;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>
        current_input_gain, current_output_gain, current_amp_master_gain;
    std::atomic<float>* input_gain_parameter = nullptr;
    std::atomic<float>* output_gain_parameter = nullptr;
    std::atomic<float>* amp_master_gain_parameter = nullptr;
    bool isAmpBypassed = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAudioProcessor)
};

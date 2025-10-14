#pragma once

#include "dsp/amp_eq.h"
#include "dsp/compressor.h"
#include "dsp/ir.h"
#include "dsp/overdrives/borealis.h"
#include "dsp/overdrives/helios.h"
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

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    // Decay factor for level smoothing
    double decayFactor = 0.85f;
    juce::Value inputLevel;                // in dB
    juce::Value outputLevel;               // in dB
    juce::Value compressorGainReductionDb; // in dB
    void updateInputLevel(juce::AudioBuffer<float>& buffer);
    void updateOutputLevel(juce::AudioBuffer<float>& buffer);
    void applyInputGain(juce::AudioBuffer<float>& buffer);
    void applyOutputGain(juce::AudioBuffer<float>& buffer);
    void applyAmpMasterGain(juce::AudioBuffer<float>& buffer);
    double smoothLevel(double newLevel, double currentLevel);

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

    Overdrive* current_overdrive = nullptr;
    HeliosOverdrive helios_overdrive;
    BorealisOverdrive borealis_overdrive;
    HeliosOverdrive nebula_overdrive;

    AmpEQ amp_eq;

    IRConvolver irConvolver;

    float previousInputGainLinear;
    float previousOutputGainLinear;
    float previousAmpMasterGainLinear;
    std::atomic<float>* inputGainParameter = nullptr;
    std::atomic<float>* outputGainParameter = nullptr;
    std::atomic<float>* ampMasterGainParameter = nullptr;
    bool isAmpBypassed = false;

    std::vector<Overdrive*> overdrives = {
        &helios_overdrive, &borealis_overdrive, &nebula_overdrive
    };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAudioProcessor)
};

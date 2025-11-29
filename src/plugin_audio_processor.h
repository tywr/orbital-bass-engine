#pragma once

#include "dsp/chorus.h"
#include "dsp/compressor.h"
#include "dsp/eq.h"
#include "dsp/ir.h"
#include "dsp/overdrives/helios.h"
#include "dsp/overdrives/overdrive.h"
#include "dsp/pitch_detector.h"
#include "dsp/synth_voices.h"
#include "preset_manager.h"
#include "session_manager.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
class PluginAudioProcessor final
    : public juce::AudioProcessor,
      public juce::AudioProcessorValueTreeState::Listener,
      public juce::ValueTree::Listener
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
    juce::Value currentPitch;              // in Hz
    void updateInputLevel(juce::AudioBuffer<float>& buffer);
    void updateOutputLevel(juce::AudioBuffer<float>& buffer);
    void applyGain(std::atomic<float>*, float&, juce::AudioBuffer<float>&);
    void setTunerBypass(bool stb)
    {
        is_tuner_bypassed = stb;
    }

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

    PresetManager& getPresetManager() { return presetManager; }
    SessionManager& getSessionManager() { return sessionManager; }

  private:
    juce::AudioProcessorValueTreeState parameters;
    juce::AudioProcessorValueTreeState::ParameterLayout parameterLayout;
    using ParamHandler = std::function<void(float)>;
    std::unordered_map<std::string, ParamHandler> parameterHandlers;

    Compressor compressor;
    EQ eq;
    IRConvolver irConvolver;
    Chorus chorus;
    SynthVoices synth_voices;
    PitchDetector pitch_detector;

    HeliosOverdrive overdrive;

    float smoothing_time = 0.05f;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>
        current_input_gain, current_output_gain, current_amp_master_gain;
    std::atomic<float>* input_gain_parameter = nullptr;
    std::atomic<float>* output_gain_parameter = nullptr;
    std::atomic<float>* amp_master_gain_parameter = nullptr;
    std::atomic<float>* amp_bypass_parameter = nullptr;
    std::atomic<float>* ir_bypass_parameter = nullptr;
    std::atomic<float>* compressor_bypass_parameter = nullptr;
    std::atomic<float>* chorus_bypass_parameter = nullptr;
    std::atomic<float>* eq_bypass_parameter = nullptr;
    std::atomic<float>* synth_bypass_parameter = nullptr;
    bool is_tuner_bypassed = true;

    PresetManager presetManager;
    SessionManager sessionManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAudioProcessor)
};

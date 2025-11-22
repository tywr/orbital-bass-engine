#include "plugin_editor.h"

#include "parameters.h"
#include "plugin_audio_processor.h"
#include <juce_dsp/juce_dsp.h>

//==============================================================================
PluginAudioProcessor::PluginAudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
      ),
      parameters(
          *this, nullptr, juce::Identifier("PluginParameters"),
          createParameterLayout()
      )
{
    parameters.state.setProperty("ir_filepath", juce::String(""), nullptr);
    parameters.state.addListener(this);

    input_gain_parameter = parameters.getRawParameterValue("input_gain_db");
    output_gain_parameter = parameters.getRawParameterValue("output_gain_db");
    amp_master_gain_parameter = parameters.getRawParameterValue("amp_master");
    amp_bypass_parameter = parameters.getRawParameterValue("amp_bypass");
    ir_bypass_parameter = parameters.getRawParameterValue("ir_bypass");
    chorus_bypass_parameter = parameters.getRawParameterValue("chorus_bypass");
    eq_bypass_parameter = parameters.getRawParameterValue("eq_bypass");
    compressor_bypass_parameter =
        parameters.getRawParameterValue("compressor_bypass");
    synth_bypass_parameter = parameters.getRawParameterValue("synth_bypass");

    for (auto* p : getParameters())
    {
        if (auto* param = dynamic_cast<juce::RangedAudioParameter*>(p))
        {
            juce::String paramID = param->getParameterID();
            parameters.addParameterListener(paramID, this);
        }
    }
}

PluginAudioProcessor::~PluginAudioProcessor()
{
}

//==============================================================================
const juce::String PluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginAudioProcessor::acceptsMidi() const
{
    return false;
}

bool PluginAudioProcessor::producesMidi() const
{
    return false;
}

bool PluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are
              // 0 programs, so this should be at least 1, even if you're not
              // really implementing programs.
}

int PluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PluginAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String PluginAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void PluginAudioProcessor::changeProgramName(
    int index, const juce::String& newName
)
{
    juce::ignoreUnused(index, newName);
}
//==============================================================================
void PluginAudioProcessor::parameterChanged(
    const juce::String& parameterID, float newValue
)
{
    setParameterValue(parameterID, newValue);
}

void PluginAudioProcessor::prepareParameters()
{
    // size_t amp_index = static_cast<size_t>(
    //     parameters.getRawParameterValue("amp_type")->load()
    // );

    // iterate over all parameters to set their initial values
    for (auto* p : getParameters())
    {
        if (auto* fparam = dynamic_cast<juce::AudioParameterFloat*>(p))
        {
            juce::String paramID = fparam->getParameterID();
            float v = fparam->get();
            setParameterValue(paramID, v);
        }
        else if (auto* cparam = dynamic_cast<juce::AudioParameterChoice*>(p))
        {
            juce::String paramID = cparam->getParameterID();
            float v = cparam->getIndex();
            setParameterValue(paramID, v);
        }
        else if (auto* rparam = dynamic_cast<juce::RangedAudioParameter*>(p))
        {
            juce::String paramID = rparam->getParameterID();
            float v = rparam->getValue();
            setParameterValue(paramID, v);
        }
    }
}

//==============================================================================
void PluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    current_input_gain.reset(sampleRate, smoothing_time);
    current_input_gain.setCurrentAndTargetValue(
        juce::Decibels::decibelsToGain(input_gain_parameter->load())
    );

    current_output_gain.reset(sampleRate, smoothing_time);
    current_output_gain.setCurrentAndTargetValue(
        juce::Decibels::decibelsToGain(output_gain_parameter->load())
    );

    current_amp_master_gain.reset(sampleRate, smoothing_time);
    current_amp_master_gain.setCurrentAndTargetValue(
        juce::Decibels::decibelsToGain(amp_master_gain_parameter->load())
    );

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32)samplesPerBlock;
    spec.numChannels = (juce::uint32)getTotalNumOutputChannels();

    synth_voices.prepare(spec);

    compressor.prepare(spec);
    eq.prepare(spec);
    irConvolver.prepare(spec);
    chorus.prepare(spec);
    overdrive.prepare(spec);
    pitch_detector.prepare(spec);
    prepareParameters();
}

void PluginAudioProcessor::releaseResources()
{
}

bool PluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts
) const
{
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

//==============================================================================
// Main Proces Block Function !
//==============================================================================
void PluginAudioProcessor::applyGain(
    std::atomic<float>* param, float& current_gain,
    juce::AudioBuffer<float>& buffer
)
{
    float gain = juce::Decibels::decibelsToGain(param->load());
    if (!juce::approximatelyEqual(current_gain, gain))
    {
        float new_gain = current_gain + (gain - current_gain) * 0.1f;
        buffer.applyGainRamp(0, buffer.getNumSamples(), current_gain, new_gain);
        current_gain = new_gain;
    }
    else
    {
        buffer.applyGain(current_gain);
    }
}

void PluginAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages
)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int num_samples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, num_samples);

    // Overwrite left channel of buffer with mono signal
    auto* left = buffer.getReadPointer(0);
    auto* right = buffer.getReadPointer(1);
    auto* mono_left = buffer.getWritePointer(0);
    auto* mono_right = buffer.getWritePointer(1);

    for (int i = 0; i < num_samples; ++i)
    {
        mono_left[i] = left[i] + right[i];
        mono_right[i] = 0.0f;
    }

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    current_input_gain.setTargetValue(
        juce::Decibels::decibelsToGain(input_gain_parameter->load())
    );
    current_input_gain.applyGain(buffer, num_samples);
    updateInputLevel(buffer);

    if (!is_tuner_bypassed)
    {
        float pitch = pitch_detector.getPitch(context);
        currentPitch.setValue(pitch);
        std::cout << "Detected Pitch: " << pitch << std::endl;
    }
    else
    {
        currentPitch.setValue(0.0f);
    }

    if (synth_bypass_parameter->load() < 0.5f)
        synth_voices.process(context);

    if (compressor_bypass_parameter->load() < 0.5f)
    {
        compressor.process(context);
        compressorGainReductionDb.setValue(compressor.getGainReductionDb());
    }

    if (amp_bypass_parameter->load() < 0.5f)
    {
        overdrive.process(context);
        current_amp_master_gain.setTargetValue(
            juce::Decibels::decibelsToGain(amp_master_gain_parameter->load())
        );
        current_amp_master_gain.applyGain(buffer, num_samples);
    }

    if (eq_bypass_parameter->load() < 0.5f)
        eq.process(context);

    // Copy mono signal back to both left and right channels
    buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());

    if (chorus_bypass_parameter->load() < 0.5f)
        chorus.process(context);
    if (ir_bypass_parameter->load() < 0.5f)
        irConvolver.process(context);

    current_output_gain.setTargetValue(
        juce::Decibels::decibelsToGain(output_gain_parameter->load())
    );
    current_output_gain.applyGain(buffer, num_samples);
    updateOutputLevel(buffer);
}

//==============================================================================
// Process Block Helper functions
//==============================================================================

void PluginAudioProcessor::updateInputLevel(juce::AudioBuffer<float>& buffer)
{
    // Set inputLevel value for metering
    double peakInput = buffer.getMagnitude(0, 0, buffer.getNumSamples());
    inputLevel.setValue(peakInput);
}

void PluginAudioProcessor::updateOutputLevel(juce::AudioBuffer<float>& buffer)
{
    // Set outputLevel value for metering
    double peakOutput = buffer.getMagnitude(0, 0, buffer.getNumSamples());
    outputLevel.setValue(peakOutput);
}

bool PluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginAudioProcessor::createEditor()
{
    return new PluginEditor(*this, parameters);
}

void PluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginAudioProcessor::setStateInformation(
    const void* data, int sizeInBytes
)
{
    std::unique_ptr<juce::XmlElement> xmlState(
        getXmlFromBinary(data, sizeInBytes)
    );
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginAudioProcessor();
}

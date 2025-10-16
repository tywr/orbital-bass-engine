#include "plugin_editor.h"

#include "parameters.h"
#include "plugin_audio_processor.h"
#include <juce_dsp/juce_dsp.h>

//==============================================================================
PluginAudioProcessor::PluginAudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::mono(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
      ),
      parameters(
          *this, nullptr, juce::Identifier("PluginParameters"),
          createParameterLayout()
      )
{
    parameters.state.setProperty("ir_filepath", juce::String(""), nullptr);

    input_gain_parameter = parameters.getRawParameterValue("input_gain_db");
    output_gain_parameter = parameters.getRawParameterValue("output_gain_db");
    amp_master_gain_parameter = parameters.getRawParameterValue("amp_master");
    isAmpBypassed = false;

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
    // Big function to map each parameter to its handler
    setParameterValue(parameterID, newValue);
}

void PluginAudioProcessor::prepareParameters()
{
    juce::String filepath =
        parameters.state.getProperty("ir_filepath").toString();
    irConvolver.setFilepath(filepath);

    int amp_index =
        static_cast<int>(parameters.getRawParameterValue("amp_type")->load());
    current_overdrive = overdrives[amp_index];

    // iterate over all parameters to set their initial values
    for (auto* p : getParameters())
    {
        if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(p))
        {
            DBG("Float Param: " + param->getParameterID() +
                " Value: " + juce::String(param->get()));
            juce::String paramID = param->getParameterID();
            float v = param->get();
            setParameterValue(paramID, v);
        }
        else if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(p))
        {
            juce::String paramID = param->getParameterID();
            float v = param->getIndex();
            setParameterValue(paramID, v);
        }
        else if (auto* param = dynamic_cast<juce::RangedAudioParameter*>(p))
        {
            juce::String paramID = param->getParameterID();
            float v = param->getValue();
            setParameterValue(paramID, v);
        }
    }
}

//==============================================================================
void PluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused(sampleRate, samplesPerBlock);
    current_input_gain =
        juce::Decibels::decibelsToGain(input_gain_parameter->load());
    current_output_gain =
        juce::Decibels::decibelsToGain(output_gain_parameter->load());

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32)samplesPerBlock;
    spec.numChannels = (juce::uint32)getTotalNumOutputChannels();

    compressor.prepare(spec);
    amp_eq.prepare(spec);
    irConvolver.prepare(spec);
    for (auto& overdrive : overdrives)
    {
        overdrive->prepare(spec);
    }
    prepareParameters();
}

void PluginAudioProcessor::releaseResources()
{
}

bool PluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts
) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        juce::ignoreUnused(channelData);
    }
    applyGain(input_gain_parameter, current_input_gain, buffer);
    updateInputLevel(buffer);

    // compressor.process(buffer);
    // compressorGainReductionDb.setValue(compressor.getGainReductionDb());

    current_overdrive->process(buffer);

    // amp_eq.process(buffer);
    //
    // if (!isAmpBypassed)
    //     applyAmpMasterGain(buffer);

    // irConvolver.process(buffer);

    applyGain(output_gain_parameter, current_output_gain, buffer);
    updateOutputLevel(buffer);

    // Convert mono to stereo if needed
    const float* input = buffer.getReadPointer(0);
    float* left = buffer.getWritePointer(0);
    float* right = buffer.getWritePointer(1);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float mono_sample = input[i];
        left[i] = mono_sample;
        right[i] = mono_sample;
    }
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

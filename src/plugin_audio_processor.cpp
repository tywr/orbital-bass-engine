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

    inputGainParameter = parameters.getRawParameterValue("input_gain_db");
    outputGainParameter = parameters.getRawParameterValue("output_gain_db");
    ampMasterGainParameter = parameters.getRawParameterValue("amp_master");
    isAmpBypassed = false;

    for (int i = 0; i < parameters.state.getNumProperties(); ++i)
    {
        juce::Identifier paramID = parameters.state.getPropertyName(i);
        parameters.addParameterListener(paramID.toString(), this);
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

//==============================================================================
void PluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused(sampleRate, samplesPerBlock);
    previousInputGainLinear =
        juce::Decibels::decibelsToGain(inputGainParameter->load());
    previousOutputGainLinear =
        juce::Decibels::decibelsToGain(outputGainParameter->load());

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

    juce::String filepath =
        parameters.state.getProperty("ir_filepath").toString();
    irConvolver.setFilepath(filepath);

    int amp_index =
        static_cast<int>(parameters.getRawParameterValue("amp_type")->load());
    current_overdrive = overdrives[amp_index];

    // iterate over all parameters to set their initial values
    for (auto* p : getParameters())
    {
        if (auto* param = dynamic_cast<juce::RangedAudioParameter*>(p))
        {
            juce::String paramID = param->getParameterID();
            float v = param->getValue();
            setParameterValue(paramID, v);
        }
    }
    // Set all initial values from compressor
    // compressor.setBypass(
    //     parameters.getRawParameterValue("compressor_bypass")->load() >= 0.5f
    // );
    // const float values[] = {2.0f, 4.0f, 8.0f, 12.0f, 20.0f};
    // int index = static_cast<int>(
    //     parameters.getRawParameterValue("compressor_ratio")->load()
    // );
    // compressor.setRatio(values[index]);
    // compressor.setThreshold(
    //     juce::Decibels::decibelsToGain(
    //         parameters.getRawParameterValue("compressor_threshold")->load()
    //     )
    // );
    // compressor.setLevel(
    //     juce::Decibels::decibelsToGain(
    //         parameters.getRawParameterValue("compressor_level_db")->load()
    //     )
    // );
    // compressor.setTypeFromIndex(
    //     static_cast<int>(
    //         parameters.getRawParameterValue("compressor_type")->load()
    //     )
    // );
    //
    // int amp_index =
    //     static_cast<int>(parameters.getRawParameterValue("amp_type")->load());
    // current_overdrive = overdrives[amp_index];
    //
    // // Set all initial values from overdrive
    // isAmpBypassed =
    //     parameters.getRawParameterValue("amp_bypass")->load() >= 0.5f;
    // for (auto& overdrive : overdrives)
    // {
    //     overdrive->setBypass(
    //         parameters.getRawParameterValue("amp_bypass")->load() >= 0.5f
    //     );
    //     overdrive->setMix(
    //         static_cast<int>(
    //             parameters.getRawParameterValue("overdrive_mix")->load()
    //         ) /
    //         100.0f
    //     );
    //     overdrive->setLevel(
    //         juce::Decibels::decibelsToGain(
    //             parameters.getRawParameterValue("overdrive_level_db")->load()
    //         )
    //     );
    //     overdrive->setDrive(
    //         parameters.getRawParameterValue("overdrive_drive")->load()
    //     );
    //     overdrive->setAttack(
    //         parameters.getRawParameterValue("overdrive_attack")->load()
    //     );
    //     overdrive->setGrunt(
    //         parameters.getRawParameterValue("overdrive_grunt")->load()
    //     );
    // }
    //
    // amp_eq.setBypass(
    //     parameters.getRawParameterValue("amp_bypass")->load() >= 0.5f
    // );
    // amp_eq.setBassGain(
    //     juce::Decibels::decibelsToGain(
    //         parameters.getRawParameterValue("amp_eq_bass")->load()
    //     )
    // );
    // amp_eq.setLowMidGain(
    //     juce::Decibels::decibelsToGain(
    //         parameters.getRawParameterValue("amp_eq_low_mid")->load()
    //     )
    // );
    // amp_eq.setHighMidGain(
    //     juce::Decibels::decibelsToGain(
    //         parameters.getRawParameterValue("amp_eq_hi_mid")->load()
    //     )
    // );
    // amp_eq.setTrebleGain(
    //     juce::Decibels::decibelsToGain(
    //         parameters.getRawParameterValue("amp_eq_treble")->load()
    //     )
    // );
    //
    // // Set all initial values for IR convolution
    // irConvolver.setBypass(
    //     parameters.getRawParameterValue("ir_bypass")->load() >= 0.5f
    // );
    // irConvolver.setMix(parameters.getRawParameterValue("ir_mix")->load());
    // irConvolver.setFilepath(
    //     parameters.state.getProperty("ir_filepath").toString()
    // );
    // irConvolver.loadIR();
}

void PluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts
) const
{
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

//==============================================================================
// Main Proces Block Function !
//==============================================================================

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

    applyInputGain(buffer);
    updateInputLevel(buffer);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        juce::ignoreUnused(channelData);
    }

    compressor.process(buffer);
    compressorGainReductionDb.setValue(compressor.getGainReductionDb());

    current_overdrive->process(buffer);

    amp_eq.process(buffer);

    if (!isAmpBypassed)
        applyAmpMasterGain(buffer);

    irConvolver.process(buffer);

    applyOutputGain(buffer);
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
    double peakInput = smoothLevel(
        buffer.getMagnitude(0, 0, buffer.getNumSamples()), inputLevel.getValue()
    );
    inputLevel.setValue(peakInput);
}

void PluginAudioProcessor::updateOutputLevel(juce::AudioBuffer<float>& buffer)
{
    // Set outputLevel value for metering
    double peakOutput = smoothLevel(
        buffer.getMagnitude(0, 0, buffer.getNumSamples()),
        outputLevel.getValue()
    );
    outputLevel.setValue(peakOutput);
}

void PluginAudioProcessor::applyInputGain(juce::AudioBuffer<float>& buffer)
{
    auto currentInputGainLinear =
        juce::Decibels::decibelsToGain(inputGainParameter->load());
    if (juce::approximatelyEqual(
            currentInputGainLinear, previousInputGainLinear
        ))
    {
        buffer.applyGain(currentInputGainLinear);
    }
    else
    {
        buffer.applyGainRamp(
            0, buffer.getNumSamples(), previousInputGainLinear,
            currentInputGainLinear
        );
        previousInputGainLinear = currentInputGainLinear;
    }
}

void PluginAudioProcessor::applyOutputGain(juce::AudioBuffer<float>& buffer)
{
    // Apply output gain with smoothing
    auto currentOutputGainLinear =
        juce::Decibels::decibelsToGain(outputGainParameter->load());
    if (juce::approximatelyEqual(
            currentOutputGainLinear, previousOutputGainLinear
        ))
    {
        buffer.applyGain(currentOutputGainLinear);
    }
    else
    {
        buffer.applyGainRamp(
            0, buffer.getNumSamples(), previousOutputGainLinear,
            currentOutputGainLinear
        );
        previousOutputGainLinear = currentOutputGainLinear;
    }
}

void PluginAudioProcessor::applyAmpMasterGain(juce::AudioBuffer<float>& buffer)
{
    // Apply output gain with smoothing
    auto currentAmpMasterGainLinear =
        juce::Decibels::decibelsToGain(ampMasterGainParameter->load());
    if (juce::approximatelyEqual(
            currentAmpMasterGainLinear, previousAmpMasterGainLinear
        ))
    {
        buffer.applyGain(currentAmpMasterGainLinear);
    }
    else
    {
        buffer.applyGainRamp(
            0, buffer.getNumSamples(), previousAmpMasterGainLinear,
            currentAmpMasterGainLinear
        );
        previousAmpMasterGainLinear = currentAmpMasterGainLinear;
    }
}

double PluginAudioProcessor::smoothLevel(double newLevel, double currentLevel)
{
    if (newLevel > currentLevel)
        return newLevel;
    else
        return decayFactor * currentLevel;
}

//==============================================================================

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

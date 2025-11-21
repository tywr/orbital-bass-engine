#include "pitch_detector.h"
#include <algorithm>

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

void PitchDetector::reset()
{
}

void PitchDetector::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
    reset();
}

float PitchDetector::getPitch(const juce::dsp::ProcessContextReplacing<float>& context)
{
    return 440.0f;
}

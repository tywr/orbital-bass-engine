#include "pitch_detector.h"
#include <algorithm>
#include <cmath>

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

void PitchDetector::reset()
{
    std::fill(inputBuffer.begin(), inputBuffer.end(), 0.0f);
    std::fill(yinBuffer.begin(), yinBuffer.end(), 0.0f);
    writeIndex = 0;
    detectedPitch = 0.0f;
}

void PitchDetector::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
    inputBuffer.resize(kBufferSize);
    yinBuffer.resize(kBufferSize / 2);
    reset();
}

float PitchDetector::getPitch(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    auto& inputBlock = context.getInputBlock();
    if (inputBlock.getNumChannels() == 0)
        return detectedPitch;

    auto numSamples = inputBlock.getNumSamples();
    auto* samples = inputBlock.getChannelPointer(0);

    if (samples == nullptr)
        return detectedPitch;

    // Fill the circular buffer
    for (size_t i = 0; i < numSamples; ++i)
    {
        inputBuffer[writeIndex] = samples[i];
        writeIndex = (writeIndex + 1) % kBufferSize;
    }

    // Only detect when buffer wraps around (reduces CPU usage)
    if (writeIndex < numSamples)
    {
        detectedPitch = detectPitchYIN();
    }

    // Mute the output while tuning
    auto& outputBlock = context.getOutputBlock();
    outputBlock.clear();

    return detectedPitch;
}

float PitchDetector::detectPitchYIN()
{
    const size_t halfBuffer = kBufferSize / 2;

    // Step 1 & 2: Compute difference function
    yinBuffer[0] = 1.0f;
    float runningSum = 0.0f;

    for (size_t tau = 1; tau < halfBuffer; ++tau)
    {
        float delta = 0.0f;
        for (size_t i = 0; i < halfBuffer; ++i)
        {
            // Access circular buffer correctly, starting from writeIndex
            // (oldest sample)
            float diff = inputBuffer[(writeIndex + i) % kBufferSize] -
                         inputBuffer[(writeIndex + i + tau) % kBufferSize];
            delta += diff * diff;
        }

        // Step 3: Cumulative mean normalized difference
        runningSum += delta;
        yinBuffer[tau] = delta * tau / runningSum;
    }

    // Step 4: Absolute threshold
    size_t tauEstimate = 0;
    for (size_t tau = 2; tau < halfBuffer; ++tau)
    {
        if (yinBuffer[tau] < kYinThreshold)
        {
            // Find the local minimum
            while (tau + 1 < halfBuffer && yinBuffer[tau + 1] < yinBuffer[tau])
            {
                ++tau;
            }
            tauEstimate = tau;
            break;
        }
    }

    if (tauEstimate == 0)
        return 0.0f;

    // Step 5: Parabolic interpolation for better precision
    float betterTau;
    if (tauEstimate > 0 && tauEstimate < halfBuffer - 1)
    {
        float s0 = yinBuffer[tauEstimate - 1];
        float s1 = yinBuffer[tauEstimate];
        float s2 = yinBuffer[tauEstimate + 1];
        betterTau = tauEstimate + (s2 - s0) / (2.0f * (2.0f * s1 - s2 - s0));
    }
    else
    {
        betterTau = static_cast<float>(tauEstimate);
    }

    return static_cast<float>(processSpec.sampleRate) / betterTau;
}

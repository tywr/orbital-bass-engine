#include "pitch_shifter.h"
#include <algorithm>

#include "../../modules/rubberband/rubberband/RubberBandLiveShifter.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

void PitchShifter::reset()
{
    shifter->reset();
}

void PitchShifter::prepare(const juce::dsp::ProcessSpec& spec)
{
    processSpec = spec;
    shifter = std::make_unique<RubberBand::RubberBandLiveShifter>(
        (size_t)spec.sampleRate, (size_t)1,
        RubberBand::RubberBandLiveShifter::Option::OptionWindowShort | 
            RubberBand::RubberBandLiveShifter::Option::OptionProcessRealTime
    );
    reset();
    // shifter->setPitchScale(1.0f)
    shifter->setPitchScale(std::pow(2.0f, static_cast<float>(2.0f / 12.0f)));
    cache_buffer.setSize(1, (int)spec.maximumBlockSize * 4, false, false, true);
}

void PitchShifter::process(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    auto& block = context.getOutputBlock();
    const size_t num_samples = block.getNumSamples();

    const float* in_ptrs[1] = {block.getChannelPointer(0)};
    const float* const* in = in_ptrs;
    auto* out = cache_buffer.getArrayOfWritePointers();
    shifter->shift(in, out);

    auto* ch = block.getChannelPointer(0);
    for (size_t i = 0; i < num_samples; ++i)
    {
        ch[i] = out[0][i];
    }
}

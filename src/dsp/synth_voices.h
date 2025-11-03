#pragma once

#include "synth_voices/octave_voice.h"
#include "synth_voices/square_voice.h"
#include "synth_voices/triangle_voice.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class SynthVoices : juce::dsp::ProcessorBase
{
  public:
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(
        const juce::dsp::ProcessContextReplacing<float>& context
    ) override;
    void reset() override;
    void updateFilters();

    void setRawLevel(float lvl)
    {
        float v = juce::jlimit(0.0f, 2.0f, lvl);
        raw_level.setTargetValue(v);
        raw_raw_level = v;
    }
    void setSquareLevel(float lvl)
    {
        float v = juce::jlimit(0.0f, 2.0f, lvl);
        square_level.setTargetValue(v);
        raw_square_level = v;
    }
    void setTriangleLevel(float lvl)
    {
        float v = juce::jlimit(0.0f, 2.0f, lvl);
        triangle_level.setTargetValue(v);
        raw_triangle_level = v;
    }
    void setOctaveLevel(float lvl)
    {
        float v = juce::jlimit(0.0f, 2.0f, lvl);
        octave_level.setTargetValue(v);
        raw_octave_level = v;
    }
    void setMasterLevel(float lvl)
    {
        float v = juce::jlimit(0.0f, 2.0f, lvl);
        master_level.setTargetValue(v);
        raw_master_level = v;
    }

  private:
    SquareVoice square_voice;
    TriangleVoice triangle_voice;
    OctaveVoice octave_voice;

    juce::dsp::ProcessSpec processSpec{44100.0, 0, 0};
    float smoothing_time = 0.05f;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> raw_level,
        square_level, triangle_level, octave_level, master_level;
    float raw_raw_level = 1.0f, raw_square_level = 1.0f,
          raw_triangle_level = 1.0f, raw_octave_level = 1.0f,
          raw_master_level = 1.0f;

    juce::AudioBuffer<float> square_buffer, triangle_buffer, octave_buffer;
};

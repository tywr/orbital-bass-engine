#pragma once

#include "preset_manager.h"
#include <array>
#include <cstring>

// Normalized (0-1) parameter defaults and factory preset overrides.
// Normalized values computed via JUCE's NormalisableRange::convertTo0to1,
// i.e. ((value - start) / (end - start)) ^ (1 / skewFactor).
namespace FactoryPresets
{

struct ParamDefault
{
    const char* id;
    float value; // normalized 0-1
};

// All parameter defaults in normalized form.
// Order must match parameters.h (order doesn't actually matter for matching,
// but keeping them aligned helps readability).
static const ParamDefault kDefaults[] = {
    // Gains                        actual: 0 dB
    { "input_gain_db",          0.8775f },
    { "output_gain_db",         0.7803f },
    // Compressor                   threshold:-24 ratio:4 attack:10ms release:100ms
    { "compressor_bypass",      0.0f   },
    { "compressor_hpf",         0.0f   },
    { "compressor_threshold",   0.5f   },
    { "compressor_ratio",       0.1111f},
    { "compressor_attack",      0.0003f},
    { "compressor_release",     0.0002f},
    { "compressor_level_db",    0.0f   },
    { "compressor_mix",         0.5f   },
    // Amp                          master:0 dB, amp active (bypass=false)
    { "amp_master",             0.6667f},
    { "amp_bypass",             0.0f   },
    { "overdrive_level_db",     0.8889f},
    { "overdrive_drive",        0.5f   },
    { "overdrive_grunt",        0.5f   },
    { "overdrive_attack",       0.5f   },
    { "overdrive_era",          0.5f   },
    { "overdrive_mix",          0.5f   },
    // Chorus                       chorus active (bypass=false)
    { "chorus_bypass",          0.0f   },
    { "chorus_mix",             0.5f   },
    { "chorus_rate",            0.5f   },
    { "chorus_depth",           0.5f   },
    { "chorus_crossover",       0.0073f},
    // IR / cabinet                 B15, mix 50%, level -18 dB
    { "ir_bypass",              0.0f   },
    { "ir_mix",                 0.5f   },
    { "ir_type",                0.0f   },
    { "ir_level",               0.375f },
    // Synth (bypassed by default)
    { "synth_bypass",           1.0f   },
    { "synth_octave_level",     0.6667f},
    { "synth_square_level",     0.6667f},
    { "synth_triangle_level",   0.6667f},
    { "synth_raw_level",        0.6667f},
    { "synth_master_level",     0.6667f},
    // EQ (bypassed by default)
    { "eq_bypass",              1.0f   },
    { "eq_low_shelf_gain",      0.5f   },
    { "eq_low_shelf_freq",      0.5f   },
    { "eq_low_mid_freq",        0.3333f},
    { "eq_low_mid_q",           0.1538f},
    { "eq_low_mid_gain",        0.5f   },
    { "eq_high_mid_freq",       0.4118f},
    { "eq_high_mid_q",          0.1538f},
    { "eq_high_mid_gain",       0.5f   },
    { "eq_high_shelf_gain",     0.5f   },
    { "eq_high_shelf_freq",     0.5f   },
    { "eq_lpf",                 0.2222f},
};

// ir_type choice indices (6 cabs: B15=0, EBS410=1, PPC212=2, SVT810=3, TC410=4, XL410=5)
// normalized = index / (numChoices - 1) = index / 5.0
static constexpr float kIR_B15    = 0.0f;   // index 0
static constexpr float kIR_EBS410 = 0.2f;   // index 1
static constexpr float kIR_PPC212 = 0.4f;   // index 2
static constexpr float kIR_SVT810 = 0.6f;   // index 3
static constexpr float kIR_TC410  = 0.8f;   // index 4
static constexpr float kIR_XL410  = 1.0f;   // index 5

struct Override
{
    const char* id;
    float value;
};

struct PresetDef
{
    const char* name;
    const Override* overrides;
    int numOverrides;
};

// ── Preset definitions ────────────────────────────────────────────────────────

// 1. Clean — flat tone, amp bypassed, SVT810 cab
static const Override kCleanOverrides[] = {
    { "amp_bypass",         1.0f         },  // bypass amp/drive
    { "chorus_bypass",      1.0f         },  // bypass chorus
    { "compressor_mix",     0.6f         },  // slightly more compression in mix
    { "ir_type",            kIR_SVT810   },  // SVT810 cabinet
};

// 2. Punchy — heavy compression, amp bypassed, B15 cab
static const Override kPunchyOverrides[] = {
    { "amp_bypass",         1.0f         },
    { "chorus_bypass",      1.0f         },
    { "compressor_threshold", 0.25f      },  // -36 dB: ((-36+48)/48)
    { "compressor_ratio",   0.3333f      },  // 8:1: (8-2)/18
    { "compressor_mix",     0.75f        },
    { "ir_type",            kIR_B15      },  // B15 cabinet
};

// 3. Driven — amp active with more drive and grit, TC410 cab
static const Override kDrivenOverrides[] = {
    { "chorus_bypass",      1.0f         },
    { "overdrive_drive",    0.7f         },  // 7/10
    { "overdrive_mix",      0.7f         },
    { "compressor_mix",     0.6f         },
    { "ir_type",            kIR_TC410    },  // TC410 cabinet
};

// 4. Scooped — mid scoop EQ, amp bypassed, SVT810 cab
static const Override kScoopedOverrides[] = {
    { "amp_bypass",         1.0f         },
    { "chorus_bypass",      1.0f         },
    { "eq_bypass",          0.0f         },  // enable EQ
    { "eq_low_mid_gain",    0.25f        },  // -6 dB: (-6+12)/24
    { "eq_high_mid_gain",   0.25f        },  // -6 dB
    { "compressor_threshold", 0.625f     },  // -18 dB: ((-18+48)/48)
    { "compressor_ratio",   0.2222f      },  // 6:1: (6-2)/18
    { "ir_type",            kIR_SVT810   },
};

// 5. Airy — chorus + bright high shelf EQ, amp bypassed, EBS410 cab
static const Override kAiryOverrides[] = {
    { "amp_bypass",         1.0f         },
    { "eq_bypass",          0.0f         },  // enable EQ
    { "eq_high_shelf_gain", 0.625f       },  // +3 dB: (3+12)/24
    { "ir_type",            kIR_EBS410   },  // EBS410 cabinet
};

static const PresetDef kPresetDefs[5] = {
    { "Clean",   kCleanOverrides,   4 },
    { "Punchy",  kPunchyOverrides,  6 },
    { "Driven",  kDrivenOverrides,  5 },
    { "Scooped", kScoopedOverrides, 7 },
    { "Airy",    kAiryOverrides,    4 },
};

// ── Builder ───────────────────────────────────────────────────────────────────

inline Preset buildPreset (const PresetDef& def)
{
    juce::ValueTree state ("PluginParameters");
    state.setProperty ("ir_filepath",           juce::String (""), nullptr);
    state.setProperty ("session_folder_path",   juce::String (""), nullptr);
    state.setProperty ("root_folder_path",      juce::String (""), nullptr);

    const int numDefaults = static_cast<int> (std::size (kDefaults));
    for (int i = 0; i < numDefaults; ++i)
    {
        float v = kDefaults[i].value;

        for (int j = 0; j < def.numOverrides; ++j)
        {
            if (std::strcmp (def.overrides[j].id, kDefaults[i].id) == 0)
            {
                v = def.overrides[j].value;
                break;
            }
        }

        juce::ValueTree param ("PARAM");
        param.setProperty ("id",    juce::String (kDefaults[i].id), nullptr);
        param.setProperty ("value", v,                               nullptr);
        state.appendChild (param, nullptr);
    }

    return Preset (juce::String (def.name), state);
}

inline std::array<Preset, 5> build()
{
    return {
        buildPreset (kPresetDefs[0]),
        buildPreset (kPresetDefs[1]),
        buildPreset (kPresetDefs[2]),
        buildPreset (kPresetDefs[3]),
        buildPreset (kPresetDefs[4]),
    };
}

} // namespace FactoryPresets

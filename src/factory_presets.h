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
// Baseline matches the "Clean" preset (Precision/preset_1.xml).
static const ParamDefault kDefaults[] = {
    // Gains                        input: 0.9 dB, output: 12 dB
    {"input_gain_db",        0.0f   },
    {"output_gain_db",       6.0f   },
    {"compressor_bypass",    0.0f   },
    {"compressor_hpf",       40.0f  },
    {"compressor_threshold", -28.0f },
    {"compressor_ratio",     4.0f   },
    {"compressor_attack",    10.0f  },
    {"compressor_release",   100.0f },
    {"compressor_level_db",  1.0f   },
    {"compressor_mix",       1.0f   },
    {"amp_bypass",           1.0f   },
    {"amp_master",           0.0f   },
    {"overdrive_level_db",   -8.0f  },
    {"overdrive_drive",      1.0f   },
    {"overdrive_grunt",      10.0f  },
    {"overdrive_attack",     5.0f   },
    {"overdrive_era",        0.0f   },
    {"overdrive_mix",        0.5f   },
    {"chorus_bypass",        1.0f   },
    {"chorus_mix",           0.17f  },
    {"chorus_rate",          0.5f   },
    {"chorus_depth",         0.5733f},
    {"chorus_crossover",     0.0142f},
    {"ir_bypass",            0.0f   },
    {"ir_mix",               0.99f  },
    {"ir_type",              0.0f   },
    {"ir_level",             -18.0f },
    {"eq_bypass",            1.0f   },
    {"eq_low_shelf_gain",    0.5f   },
    {"eq_low_shelf_freq",    0.5f   },
    {"eq_low_mid_freq",      0.595f },
    {"eq_low_mid_q",         0.1538f},
    {"eq_low_mid_gain",      0.5f   },
    {"eq_high_mid_freq",     0.4118f},
    {"eq_high_mid_q",        0.1538f},
    {"eq_high_mid_gain",     0.5f   },
    {"eq_high_shelf_gain",   0.5f   },
    {"eq_high_shelf_freq",   0.5f   },
    {"eq_lpf",               0.3184f},
};

static constexpr float kIR_B15 = 0.0f;    // index 0
static constexpr float kIR_EBS410 = 1.0f; // index 1
static constexpr float kIR_PPC212 = 2.0f; // index 2
static constexpr float kIR_SVT810 = 3.0f; // index 3
static constexpr float kIR_TC410 = 4.0f;  // index 4
static constexpr float kIR_XL410 = 5.0f;  // index 5

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

// ── Preset definitions
// ────────────────────────────────────────────────────────

// 1. Clean — flat tone, amp bypassed, SVT810 cab
static const Override kCleanOverrides[] = {
    {"amp_bypass",    0.0f      }, // bypass amp/drive
    {"chorus_bypass", 1.0f      }, // bypass chorus
    {"eq_bypass",     1.0f      }, // bypass chorus
    {"ir_bypass",     1.0f      }, // bypass ir
    {"ir_mix",        0.5f      },
    {"ir_type",       kIR_EBS410}, // EBS410 cabinet
};

// 2. Motown - wooly, saturated, B15 flavour
static const Override kMotownOverrides[] = {
    {"amp_bypass",         0.0f   },
    {"chorus_bypass",      1.0f   },
    {"overdrive_level_db", -12.0f },
    {"overdrive_drive",    3.0f   },
    {"overdrive_attack",   2.0f   },
    {"ir_mix",             0.25f  },
    {"eq_bypass",          1.0f   },
    {"ir_mix",             0.5f   },
    {"ir_type",            kIR_B15}, // B15 cabinet
};

// 3. Driven — amp active with more drive and grit, TC410 cab
static const Override kDrivenOverrides[] = {
    {"amp_bypass",           0.0f      },
    {"compressor_threshold", -32.0f    },
    {"compressor_attack",    5.0f      },
    {"chorus_bypass",        1.0f      },
    {"overdrive_level_db",   -18.0f    },
    {"overdrive_drive",      4.0f      },
    {"overdrive_attack",     5.0f      },
    {"overdrive_era",        5.0f      },
    {"ir_mix",               0.4f      },
    {"eq_bypass",            1.0f      },
    {"ir_type",              kIR_SVT810}, // SVT cabinet
};

// 4. SVT / Arena Rock — mid scoop EQ, amp bypassed, SVT810 cab
static const Override kSVTOverrides[] = {
    {"amp_bypass",           0.0f      },
    {"compressor_threshold", -32.0f    },
    {"compressor_attack",    3.0f      },
    {"chorus_bypass",        0.0f      },
    {"overdrive_level_db",   -22.0f    },
    {"overdrive_drive",      5.0f      },
    {"overdrive_attack",     6.0f      },
    {"overdrive_era",        9.0f      },
    {"ir_mix",               0.8f      },
    {"eq_bypass",            1.0f      },
    {"chorus_crossover",     250.0f    },
    {"chorus_mix",           0.22f     },
    {"ir_type",              kIR_SVT810}, // SVT cabinet
};

// 5. Metal — Aggressive drive, PCP212 cab
static const Override kMetalOverrides[] = {
    {"amp_bypass",           0.0f      },
    {"compressor_threshold", -30.0f    },
    {"compressor_attack",    2.0f      },
    {"overdrive_level_db",   -25.0f    },
    {"overdrive_drive",      10.0f     },
    {"overdrive_attack",     8.0f      },
    {"overdrive_era",        10.0f     },
    {"overdrive_grunt",      5.0f      },
    {"ir_mix",               0.9f      },
    {"eq_bypass",            1.0f      },
    {"ir_type",              kIR_PPC212}, // SVT cabinet
};

static const PresetDef kPresetDefs[5] = {
    {"Clean",  kCleanOverrides,  static_cast<int>(std::size(kCleanOverrides)) },
    {"Motown", kMotownOverrides, static_cast<int>(std::size(kMotownOverrides))},
    {"Driven", kDrivenOverrides, static_cast<int>(std::size(kDrivenOverrides))},
    {"SVT",    kSVTOverrides,    static_cast<int>(std::size(kSVTOverrides))   },
    {"Metal",  kMetalOverrides,  static_cast<int>(std::size(kMetalOverrides)) },
};

// ── Builder
// ───────────────────────────────────────────────────────────────────

inline Preset buildPreset(const PresetDef& def)
{
    juce::ValueTree state("PluginParameters");
    state.setProperty("ir_filepath", juce::String(""), nullptr);
    state.setProperty("session_folder_path", juce::String(""), nullptr);
    state.setProperty("root_folder_path", juce::String(""), nullptr);

    const int numDefaults = static_cast<int>(std::size(kDefaults));
    for (int i = 0; i < numDefaults; ++i)
    {
        float v = kDefaults[i].value;

        for (int j = 0; j < def.numOverrides; ++j)
        {
            if (std::strcmp(def.overrides[j].id, kDefaults[i].id) == 0)
            {
                v = def.overrides[j].value;
                break;
            }
        }

        juce::ValueTree param("PARAM");
        param.setProperty("id", juce::String(kDefaults[i].id), nullptr);
        param.setProperty("value", v, nullptr);
        state.appendChild(param, nullptr);
    }

    return Preset(juce::String(def.name), state);
}

inline std::array<Preset, 5> build()
{
    return {
        buildPreset(kPresetDefs[0]), buildPreset(kPresetDefs[1]),
        buildPreset(kPresetDefs[2]), buildPreset(kPresetDefs[3]),
        buildPreset(kPresetDefs[4]),
    };
}

} // namespace FactoryPresets

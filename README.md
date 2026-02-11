# Orbital Bass Engine

"Bass tone open-source optimization protocol: launch sequence initiated."

## Overview

Orbital Bass Engine is an open-source bass guitar audio plugin built with the JUCE framework. It provides a complete signal processing chain designed specifically for bass guitar, featuring analog-modeled compression, dual-drive amplification, and a post-amp effects rack.

The plugin processes bass signals through a carefully crafted signal chain that preserves low-end clarity while adding character, punch, and presence. All components are designed with bass frequencies in mind, ensuring tight, powerful tones without muddiness.

## Signal Chain

The audio processing flows through the following stages:

1. **Input Gain** - Initial level control
3. **Compressor** - Dynamics control with analog-modeled circuits
4. **Amp/Dual-Drive** - Overdrive and distortion with parallel signal paths
5. **EQ** - Multi-band tone shaping
6. **Chorus** - Stereo width and modulation (post-stereo conversion)
7. **IR Convolver** - Cabinet simulation using impulse responses
8. **Output Gain** - Final level control

## Components

### Compressor

The compressor features two distinct analog-modeled compression circuits:

- **FET**: Models JFET-based compression with fast attack and aggressive character. Uses custom JFET circuit modeling for punchy, colorful compression ideal for adding sustain and presence.

**Controls:**
- Threshold (-48dB to 0dB)
- Ratio (2:1, 4:1, 8:1, 12:1, 20:1)
- Level (makeup gain, 0dB to 24dB)
- Mix (parallel compression, 0% to 100%)

### Amp Dual-Drive (HeliosOverdrive)

The heart of the tone engine features a sophisticated dual-drive architecture that processes the signal through two parallel paths (VMT and B3K), then blends them for complex harmonic saturation.

**Key Features:**
- Dual parallel signal paths with independent filtering and saturation
- CMOS circuit modeling for authentic analog overdrive characteristics
- 2x oversampling for aliasing-free distortion
- Multiple pre and post-filtering stages for precise tone sculpting

**Controls:**
- **Drive**: Controls the amount of overdrive and gain staging (0-10)
- **Grunt**: Low-frequency shelf filter for bottom-end thickness and growl (0-10)
- **Attack**: High-frequency shelf filter for presence and articulation (0-10)
- **Era**: Tone filter that shapes the overall voice and character (0-10)
- **Mix**: Blend between clean and driven signals (0% to 100%)
- **Master**: Final output level control (-24dB to 12dB)

### Post-Amp Rack

After amplification, the signal passes through an effects rack:

#### EQ (Equalizer)

A 6-band parametric equalizer with carefully chosen frequencies for bass guitar, plus a variable low-pass filter.

**Bands:**
- 80 Hz (low-band)
- 250 Hz
- 500 Hz
- 1.5 kHz
- 3 kHz
- 5 kHz (high-band)

**Additional:**
- Low-pass filter (1kHz to 10kHz) for taming excessive high-end

Each band offers ±12dB of gain control with optimized Q values for musical results.

#### Chorus

A stereo chorus effect that adds width and dimension to the bass tone without losing low-end focus.

**Features:**
- Multiband processing with crossover control (prevents chorusing of sub-bass frequencies)
- Stereo LFO modulation with independent left/right channels
- Lagrange interpolation for smooth, artifact-free modulation

**Controls:**
- Rate (0.5Hz to 2.5Hz)
- Depth (modulation amount)
- Crossover (50Hz to 1kHz - frequencies below this remain mono)
- Mix (0% to 100%)

#### IR Convolver (Cabinet Simulation)

High-quality cabinet simulation using impulse responses for realistic speaker cabinet and microphone modeling.

**Features:**
- Multiple impulse response options
- Parallel processing for preserving low-end punch
- Variable mix for blending direct and processed signals

**Controls:**
- IR Type (selectable from built-in impulse responses)
- Level (-36dB to 12dB)
- Mix (0% to 100%)

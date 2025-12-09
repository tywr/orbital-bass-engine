#pragma once
#include "ImpulseResponseBinary.h"

inline juce::StringArray impulseResponseBinaryNames{
    "B15",
    "EBS410",
    "PPC212",
    "SVT810",
    "TC410",
    "XL410",
};
inline const char* impulseResponseBinaryWavFiles[] = {
    ImpulseResponseBinary::B15_wav,
    ImpulseResponseBinary::EBS410_wav,
    ImpulseResponseBinary::PPC212_wav,
    ImpulseResponseBinary::SVT810_wav,
    ImpulseResponseBinary::TC410_wav,
    ImpulseResponseBinary::XL410_wav,
};

inline const int impulseResponseBinaryWavSizes[] = {
    ImpulseResponseBinary::B15_wavSize,
    ImpulseResponseBinary::EBS410_wavSize,
    ImpulseResponseBinary::PPC212_wavSize,
    ImpulseResponseBinary::SVT810_wavSize,
    ImpulseResponseBinary::TC410_wavSize,
    ImpulseResponseBinary::XL410_wavSize,
};
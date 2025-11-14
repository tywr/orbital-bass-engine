#pragma once
#include "ImpulseResponseBinary.h"

inline juce::StringArray impulseResponseBinaryNames{
    "PPC212",
    "SVT810",
    "XL410",
};
inline const char* impulseResponseBinaryWavFiles[] = {
    ImpulseResponseBinary::PPC212_wav,
    ImpulseResponseBinary::SVT810_wav,
    ImpulseResponseBinary::XL410_wav,
};

inline const int impulseResponseBinaryWavSizes[] = {
    ImpulseResponseBinary::PPC212_wavSize,
    ImpulseResponseBinary::SVT810_wavSize,
    ImpulseResponseBinary::XL410_wavSize,
};
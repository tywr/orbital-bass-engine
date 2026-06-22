#pragma once

#include "BinaryData.h"
#include <juce_graphics/juce_graphics.h>

namespace Fonts
{

inline juce::Typeface::Ptr getTypeface()
{
    static auto typeface = juce::Typeface::createSystemTypefaceFor(
        BinaryData::NordwandMono_ttf, BinaryData::NordwandMono_ttfSize);
    return typeface;
}

inline juce::Font getFont(float height, bool bold = false)
{
    auto font = juce::Font(juce::FontOptions(getTypeface()).withHeight(height));
    if (bold)
        font = font.boldened();
    return font;
}

} // namespace Fonts

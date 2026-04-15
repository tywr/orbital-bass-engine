#pragma once

#include <juce_graphics/juce_graphics.h>
#include "BinaryData.h"

namespace Fonts
{

inline juce::Typeface::Ptr getIndexTypeface()
{
    static auto typeface = juce::Typeface::createSystemTypefaceFor(
        BinaryData::IndexRegular_ttf, BinaryData::IndexRegular_ttfSize);
    return typeface;
}

inline juce::Font getIndexFont(float height, bool bold = false)
{
    auto font = juce::Font(juce::FontOptions(getIndexTypeface()).withHeight(height));
    if (bold)
        font = font.boldened();
    return font;
}

} // namespace Fonts

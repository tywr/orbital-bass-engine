#pragma once

#include <juce_graphics/juce_graphics.h>

namespace ColourCodes
{
juce::Colour const bg = juce::Colour(0, 0, 0);
juce::Colour const bg0 = juce::Colour(10, 10, 10);
juce::Colour const bg1 = juce::Colour(20, 20, 20);
juce::Colour const bg2 = juce::Colour(30, 30, 30);
juce::Colour const bg3 = juce::Colour(40, 40, 40);
juce::Colour const grey0 = juce::Colour(46, 52, 64);
juce::Colour const grey1 = juce::Colour(59, 66, 82);
juce::Colour const grey2 = juce::Colour(67, 76, 94);
juce::Colour const grey3 = juce::Colour(76, 86, 106);
juce::Colour const blue0 = juce::Colour(143, 188, 187);
juce::Colour const blue1 = juce::Colour(136, 192, 208);
juce::Colour const blue2 = juce::Colour(129, 161, 193);
juce::Colour const blue3 = juce::Colour(94, 129, 172);
//
juce::Colour const white0 = juce::Colour(216, 222, 233);
juce::Colour const white1 = juce::Colour(229, 233, 240);
juce::Colour const white2 = juce::Colour(236, 239, 244);

juce::Colour const orange = juce::Colour(0xffff8c00);

juce::Colour const aurora_green = juce::Colour(163, 190, 140);
juce::Colour const aurora_violet = juce::Colour(180, 142, 173);
juce::Colour const aurora_orange = juce::Colour(208, 135, 112);
juce::Colour const aurora_red = juce::Colour(191, 97, 106);

juce::Colour const helios_yellow = juce::Colour(242, 165, 61);
juce::Colour const helios_orange = juce::Colour(138, 38, 27);

juce::Colour const nebula_red = juce::Colour(191, 97, 106);
juce::Colour const nebula_violet = juce::Colour(180, 142, 173);
} // namespace ColourCodes

namespace GuiColours
{
juce::Colour const DEFAULT_INACTIVE_COLOUR = ColourCodes::bg2;

juce::Colour const COMPRESSOR_ACTIVE_COLOUR_1 = ColourCodes::blue0;
juce::Colour const COMPRESSOR_ACTIVE_COLOUR_2 = ColourCodes::blue2;
juce::Colour const COMPRESSOR_BG_COLOUR = ColourCodes::bg0;
juce::Colour const COMPRESSOR_STYLING_COLOUR = ColourCodes::bg2;

juce::Colour const CHORUS_ACTIVE_COLOUR_1 = ColourCodes::aurora_violet;
juce::Colour const CHORUS_ACTIVE_COLOUR_2 = ColourCodes::aurora_green;
juce::Colour const CHORUS_BG_COLOUR = ColourCodes::bg0;
juce::Colour const CHORUS_STYLING_COLOUR = ColourCodes::bg2;

juce::Colour const EQ_ACTIVE_COLOUR_1 = ColourCodes::blue0;
juce::Colour const EQ_ACTIVE_COLOUR_2 = ColourCodes::blue1;
juce::Colour const EQ_BG_COLOUR = ColourCodes::bg0;
juce::Colour const EQ_STYLING_COLOUR = ColourCodes::bg2;

juce::Colour const AMP_BG_COLOUR = ColourCodes::bg0;

juce::Colour const RACK_COLOUR = ColourCodes::grey0;
} // namespace GuiColours

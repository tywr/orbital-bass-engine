#pragma once

inline void paintGravity(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    juce::Path boxPath;
    boxPath.addRectangle(bounds);
    g.reduceClipRegion(boxPath);

    auto ext_bounds =
        bounds.expanded(bounds.getWidth() * 1.1f, bounds.getHeight() * 1.1f);
    auto centre = ext_bounds.getCentre();

    // juce::Colour colour = juce::Colours::white;
    juce::Colour colour = GuiColours::COMPRESSOR_STYLING_COLOUR;
    g.setColour(colour);

    const int spacing = 10;           // grid spacing
    const float eventHorizon = 30.0f; // radius of black hole
    const float distortion = 5000.0f;
    const float dotSize = 2.0f;

    // compute number of dots
    int numX = static_cast<int>(std::floor(ext_bounds.getWidth() / spacing));
    int numY = static_cast<int>(std::floor(ext_bounds.getHeight() / spacing));

    // total grid size
    float totalWidth = numX * spacing;
    float totalHeight = numY * spacing;

    // start coordinates to center the grid exactly
    float startX = centre.x - totalWidth / 2.0f + spacing / 2.0f;
    float startY = centre.y - totalHeight / 2.0f + spacing / 2.0f;

    for (int iy = 0; iy < numY; ++iy)
    {
        for (int ix = 0; ix < numX; ++ix)
        {
            float x = startX + ix * spacing;
            float y = startY + iy * spacing;

            float dx = x - centre.x;
            float dy = y - centre.y;
            float dist = std::sqrt(dx * dx + dy * dy);

            juce::Point<float> pos(x, y);

            if (dist > eventHorizon)
            {
                // symmetric radial warp
                float warpedDist = dist / (1.0f + distortion / (dist * dist));
                float scale = warpedDist / dist;

                pos.x = centre.x + dx * scale;
                pos.y = centre.y + dy * scale;
            }
            else
            {
                pos = centre;
            }

            float alpha =
                juce::jlimit(0.2f, 1.0f, dist / (ext_bounds.getWidth() * 0.3f));
            g.setColour(colour.withAlpha(alpha));

            g.fillEllipse(
                pos.x - dotSize * 0.5f, pos.y - dotSize * 0.5f, dotSize, dotSize
            );
        }
    }

    // Draw the event horizon
    g.setColour(GuiColours::COMPRESSOR_BG_COLOUR);
    g.fillEllipse(
        centre.x - eventHorizon, centre.y - eventHorizon, eventHorizon * 2.0f,
        eventHorizon * 2.0f
    );
}

#pragma once
#include "../utils/perlin.h"
#include "../utils/wobbly_path.h"
#include <algorithm>
#include <cmath>

inline void paintIconNebula(
    juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour c1,
    juce::Colour c2
)
{
    auto center = bounds.getCentre();
    float maxRadius = bounds.getHeight() * 0.45f;
    const float borderThickness = 2.0f;

    g.setColour(juce::Colours::black);
    g.fillEllipse(
        center.x - maxRadius, center.y - maxRadius, maxRadius * 2.0f,
        maxRadius * 2.0f
    );

    juce::ColourGradient gradient(
        c1, center.x - maxRadius, center.y, // left side
        c2, center.x + maxRadius, center.y, // right side
        false                               // not radial, just linear
    );
    g.setGradientFill(gradient);
    g.drawEllipse(
        center.x - maxRadius, center.y - maxRadius, maxRadius * 2,
        maxRadius * 2, borderThickness
    );

    maxRadius -= borderThickness * maxRadius * 0.03f;

    juce::Path circlePath;
    circlePath.addEllipse(
        center.x - maxRadius, center.y - maxRadius, maxRadius * 2.0f,
        maxRadius * 2.0f
    );
    g.reduceClipRegion(circlePath);

    juce::ColourGradient innerGradient(
        c2, center.x, center.y - maxRadius, c1, center.x, center.y + maxRadius,
        true
    );
    g.setGradientFill(innerGradient);
    g.fillEllipse(
        center.x - maxRadius, center.y - maxRadius, maxRadius * 2, maxRadius * 2
    );

    const int numStripes = 6;
    const float gap = maxRadius * 0.6f;
    const float stripeHeight = maxRadius * 0.04f;

    for (int i = 0; i < numStripes; ++i)
    {
        float t = (float)i / (numStripes - 1);
        float yStripe = center.y + t * (maxRadius * 0.9f);

        float xStripe = center.x - maxRadius;
        g.setColour(juce::Colours::black);

        // Draw each stripe centered vertically around yStripe
        g.fillRect(
            xStripe, yStripe - stripeHeight / 2.0f, maxRadius * 2.0f,
            stripeHeight
        );
    }

    // --- Optional: subtle glow or bloom ---
    // juce::ColourGradient glow(
    //     c1.withAlpha(0.15f), center.x, center.y - maxRadius * 0.5f,
    //     c2.withAlpha(0.0f), center.x, center.y + maxRadius * 1.2f, true
    // );
    // g.setGradientFill(glow);
    // g.fillEllipse(
    //     center.x - maxRadius * 1.1f, center.y - maxRadius * 1.1f,
    //     maxRadius * 2.2f, maxRadius * 2.2f
    // );
}

inline void paintDesignNebula(
    juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour c1,
    juce::Colour c2
)
{
    juce::Graphics::ScopedSaveState state(g);

    auto center = bounds.getCentre();

    // Background noise layers
    const int numBackgroundNoiseLayers = 20;

    juce::Path boxPath;
    boxPath.addRectangle(bounds);
    g.reduceClipRegion(boxPath);

    float maxBackgroundRadius = bounds.getWidth();

    for (int i = 0; i < numBackgroundNoiseLayers; ++i)
    {
        float proportion = (float)i / (numBackgroundNoiseLayers); // 0.0 to
        float currentRadius = maxBackgroundRadius *
                              (proportion * 0.8f); // Start larger, go to max

        g.setColour(GuiColours::DEFAULT_INACTIVE_COLOUR);
        g.drawEllipse(
            center.x - currentRadius, center.y - currentRadius,
            currentRadius * 2, currentRadius * 2, 1.0f
        );
    }
    paintIconNebula(g, bounds, c1, c2);
}

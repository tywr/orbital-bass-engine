#pragma once
#include "../utils/wobbly_path.h"

inline void paintSunFigureHelios(
    juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour c1,
    juce::Colour c2
)
{
    auto center = bounds.getCentre();
    float maxRadius = bounds.getHeight() * 0.45f;

    float blackHoleOffsetX = 0.2f * maxRadius;
    float blackHoleOffsetY = -0.2f * maxRadius;
    auto blackHoleCenter =
        center.translated(blackHoleOffsetX, blackHoleOffsetY);

    // Draw the sun figure in the center
    const int borderThickness = 2.0f;
    const int numLayers = 10;
    const float noiseFrequency = 1.0f;
    const float noiseAmount = 0.3f;
    const float zOffsetStep = 0.1f;

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

    // Create a circular path for clipping
    juce::Path circlePath;
    circlePath.addEllipse(
        center.x - maxRadius, center.y - maxRadius, maxRadius * 2.0f,
        maxRadius * 2.0f
    );
    g.reduceClipRegion(circlePath);

    g.setColour(c1);
    g.fillEllipse(
        center.x - maxRadius, center.y - maxRadius, maxRadius * 2, maxRadius * 2
    );

    for (int i = 1; i < numLayers; ++i)
    {
        // Interpolate radius and colour for the current layer
        float proportion = 1.0f - (float)i / numLayers;
        float currentRadius = maxRadius * proportion;

        juce::Colour currentColour =
            c1.interpolatedWith(c2, (float)i / (numLayers - 1));

        float zOffset = i * zOffsetStep;
        auto currCenter = center.translated(
            blackHoleOffsetX * i / numLayers, blackHoleOffsetY * i / numLayers
        );
        auto path = createWobblyPath(
            currCenter, currentRadius, noiseAmount, noiseFrequency, zOffset
        );

        // Set the colour and fill the path
        g.setColour(currentColour);
        g.fillPath(path);
    }

    float holeRadius = maxRadius * 0.25f;
    juce::Path holePath = createWobblyPath(
        blackHoleCenter, holeRadius, noiseAmount * 0.5f, noiseFrequency,
        numLayers * zOffsetStep
    );
    g.setColour(juce::Colours::black);
    g.fillPath(holePath);
}

inline void paintDesignHelios(
    juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour c1,
    juce::Colour c2
)
{
    juce::Graphics::ScopedSaveState state(g);

    auto center = bounds.getCentre();

    // Background noise layers
    const int numBackgroundNoiseLayers = 25;
    const float backgroundNoiseFrequency = 1.0f;
    const float backgroundNoiseAmount = 0.3f;
    const float backgroundZOffsetStep = 0.1f;

    juce::Path boxPath;
    boxPath.addRectangle(bounds);
    g.reduceClipRegion(boxPath);

    float maxBackgroundRadius = bounds.getWidth();

    for (int i = 0; i < numBackgroundNoiseLayers; ++i)
    {
        float proportion = (float)i / (numBackgroundNoiseLayers); // 0.0 to <1.0
        float currentRadius = maxBackgroundRadius *
                              (proportion * 0.8f); // Start larger, go to max

        float zOffset =
            i * backgroundZOffsetStep +
            100.0f; // Add a large offset to distinguish from thermal noise
        auto path = createWobblyPath(
            center, currentRadius, backgroundNoiseAmount,
            backgroundNoiseFrequency, zOffset
        );

        g.setColour(GuiColours::DEFAULT_INACTIVE_COLOUR);
        g.strokePath(path, juce::PathStrokeType(1.0f));
    }
    paintSunFigureHelios(g, bounds, c1, c2);
}

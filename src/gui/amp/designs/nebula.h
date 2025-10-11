#pragma once
#include "../utils/perlin.h"
#include "../utils/wobbly_path.h"
#include <algorithm>
#include <cmath>

inline void paintSmallIconNebula(
    juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour c1,
    juce::Colour c2
)
{
    auto center = bounds.getCentre();
    float maxRadius = bounds.getHeight() * 0.45f;

    const int borderThickness = 2.0f;

    // Outer border
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
    maxRadius -= borderThickness * 2.0f;

    // Create a circular path for clipping
    juce::Path circlePath;
    circlePath.addEllipse(
        center.x - maxRadius, center.y - maxRadius, maxRadius * 2.0f,
        maxRadius * 2.0f
    );
    g.reduceClipRegion(circlePath);

    const float dotRadius = 1.0f;
    const float dotDiameter = dotRadius * 2.0f;
    PerlinNoise perlin = PerlinNoise();
    float sphere_radius = maxRadius;
    float grid_size = dotRadius * 1.5f;
    float noiseScaleX = 0.0003f;
    float noiseScaleY = 0.0003f;
    float camera_z_offset = maxRadius * 2.0f;
    float focal_length = maxRadius * 1.55f;
    juce::Random random(12345);

    for (float x = center.x - maxRadius; x < center.x + maxRadius;
         x += grid_size)
    {
        for (float y = center.y - maxRadius; y < center.y + maxRadius;
             y += grid_size)
        {
            juce::Point<float> currentPoint(x, y);
            float distFromCentre = currentPoint.getDistanceFrom(center);
            float z = std::sqrt(
                std::pow(sphere_radius, 2) - std::pow(x - center.x, 2) -
                std::pow(y - center.y, 2)
            );

            if (distFromCentre > maxRadius)
                continue;

            float centerRatio = distFromCentre / maxRadius;
            float noise =
                (float)perlin.noise(x * noiseScaleX, y * noiseScaleY, 0.0f);
            if (centerRatio > 0.9f)
            {
                noise = std::min(1.0f, noise + 0.4f * centerRatio);
            }

            float max_alpha = 0.75f;
            float min_alpha = 0.25f;
            float finalAlpha = (max_alpha - min_alpha) * noise + min_alpha;
            // z += noise * 0.15f * maxRadius * (1.0f - centerRatio);
            z += noise * 0.1f * maxRadius;
            // Get the color
            float y_proportion = (y - bounds.getY()) / bounds.getHeight();
            juce::Colour dotColour =
                c1.interpolatedWith(c2, y_proportion).withAlpha(finalAlpha);

            // Get the distance between the camera and the screen
            float dist_camera_z = camera_z_offset - z;
            float p_x =
                ((x - center.x) * focal_length) / (dist_camera_z) + center.x;
            float p_y =
                ((y - center.y) * focal_length) / (dist_camera_z) + center.y;

            g.setColour(dotColour);
            g.fillEllipse(
                p_x - dotRadius, p_y - dotRadius, 2 * dotRadius, 2 * dotRadius
            );
        }
    }
}

inline void paintIconNebula(
    juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour c1,
    juce::Colour c2
)
{
    auto center = bounds.getCentre();
    float maxRadius = bounds.getHeight() * 0.45f;

    const int borderThickness = 2.0f;
    g.setColour(juce::Colours::black);
    g.fillEllipse(
        center.x - maxRadius, center.y - maxRadius, maxRadius * 2, maxRadius * 2
    );

    // Outer border
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

    const float dotRadius = std::max(0.5f, maxRadius * 0.012f);
    const float dotDiameter = dotRadius * 2.0f;
    PerlinNoise perlin = PerlinNoise();
    float sphere_radius = maxRadius;
    float grid_size = dotRadius * 3.0f;
    float noiseScaleX = 0.08f;
    float noiseScaleY = 0.07f;
    float camera_z_offset = maxRadius * 2.0f;
    float focal_length = maxRadius * 1.55f;
    juce::Random random(12345);

    for (float x = center.x - maxRadius; x < center.x + maxRadius;
         x += grid_size)
    {
        for (float y = center.y - maxRadius; y < center.y + maxRadius;
             y += grid_size)
        {
            juce::Point<float> currentPoint(x, y);
            float distFromCentre = currentPoint.getDistanceFrom(center);
            float z = std::sqrt(
                std::pow(sphere_radius, 2) - std::pow(x - center.x, 2) -
                std::pow(y - center.y, 2)
            );

            if (distFromCentre > maxRadius)
                continue;

            float centerRatio = distFromCentre / maxRadius;
            float noise =
                (float)perlin.noise(x * noiseScaleX, y * noiseScaleY, 0.0f);
            if (centerRatio > 0.9f)
            {
                noise = std::min(1.0f, noise + 0.4f * centerRatio);
            }

            float max_alpha = 0.75f;
            float min_alpha = 0.25f;
            float finalAlpha = (max_alpha - min_alpha) * noise + min_alpha;
            // z += noise * 0.15f * maxRadius * (1.0f - centerRatio);
            z += noise * 0.1f * maxRadius;
            // Get the color
            float y_proportion = (y - bounds.getY()) / bounds.getHeight();
            juce::Colour dotColour =
                c1.interpolatedWith(c2, y_proportion).withAlpha(finalAlpha);

            // Get the distance between the camera and the screen
            float dist_camera_z = camera_z_offset - z;
            float p_x =
                ((x - center.x) * focal_length) / (dist_camera_z) + center.x;
            float p_y =
                ((y - center.y) * focal_length) / (dist_camera_z) + center.y;

            g.setColour(dotColour);
            g.fillEllipse(
                p_x - dotRadius, p_y - dotRadius, 2 * dotRadius, 2 * dotRadius
            );
        }
    }
}

inline void paintDesignNebula(
    juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour c1,
    juce::Colour c2
)
{
    juce::Graphics::ScopedSaveState state(g);

    auto center = bounds.getCentre();
    float maxRadius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.45f;

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

        // Use a subtle grey colour with transparency
        juce::Colour backgroundColour = juce::Colours::darkgrey;

        g.setColour(GuiColours::DEFAULT_INACTIVE_COLOUR);
        g.drawEllipse(
            center.x - currentRadius, center.y - currentRadius,
            currentRadius * 2, currentRadius * 2, 1.0f
        );
    }
    paintIconNebula(g, bounds, c1, c2);
}

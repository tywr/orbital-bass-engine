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

    const int borderThickness = 2.0f;

    // Outer border
    juce::ColourGradient gradient(
        c1, center.x - maxRadius, center.y, // left side
        c2, center.x + maxRadius, center.y, // right side
        false                               // not radial, just linear
    );
    g.setColour(juce::Colours::black);
    g.fillEllipse(
        center.x - maxRadius, center.y - maxRadius, maxRadius * 2, maxRadius * 2
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

    // Compute the coordinates first on the unit circle, then scale up
    // to the desired radius
    const float scale_factor = 0.8f * maxRadius;
    const float dot_radius = 0.02f;
    const float grid_size = dot_radius * 2.0f;
    const float noise_scale_x = 6.0f;
    const float noise_scale_y = 8.0f;
    const float camera_z_offset = 2.5f;
    const float focal_length = 2.5f;
    // float focal_length = 1.55f;

    PerlinNoise perlin = PerlinNoise();

    for (float x = -1; x < 1; x += grid_size)
    {
        for (float y = -1; y < 1; y += grid_size)
        {
            juce::Point<float> currentPoint(x, y);
            float r = std::sqrt(x * x + y * y);
            float z = std::sqrt(1 - x * x - y * y);

            if (r > 1)
                continue;

            float noise =
                (float)perlin.noise(x * noise_scale_x, y * noise_scale_y, 0.0f);
            if (r >= 0.85f)
            {
                noise = std::min(1.0f, noise + 0.4f);
            }

            float max_alpha = 0.75f;
            float min_alpha = 0.2f;
            float finalAlpha =
                std::max(0.0f, (max_alpha - min_alpha) * noise + min_alpha);
            z += noise * 0.15f;
            // Get the color based on the y position
            juce::Colour dotColour = c1.interpolatedWith(c2, (y + 1.0f) / 2.0f)
                                         .withAlpha(finalAlpha);

            // Project the 3D points onto 2D using perspective projection
            float dist_camera_z = camera_z_offset - z;
            float x_p = (x * focal_length) / dist_camera_z;
            float y_p = (y * focal_length) / dist_camera_z;

            // Now scale up to the actual bounds size
            float x_r = center.x + x_p * scale_factor;
            float y_r = center.y + y_p * scale_factor;
            float r_r = dot_radius * scale_factor;
            g.setColour(dotColour);
            g.fillEllipse(x_r - r_r, y_r - r_r, 2 * r_r, 2 * r_r);
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

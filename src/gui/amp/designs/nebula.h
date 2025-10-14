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
    const float scale_factor = 0.73f * maxRadius;
    const float dot_radius = 0.02f;
    const float grid_size = dot_radius * 2.0f;
    const float camera_z_offset = 2.5f;
    const float focal_length = 2.5f;
    juce::Random random(123);

    PerlinNoise perlin = PerlinNoise();
    PerlinNoise perlin2 = PerlinNoise();
    perlin2.reseed(123);
    PerlinNoise perlin3 = PerlinNoise();
    perlin3.reseed(124);
    PerlinNoise perlin4 = PerlinNoise();
    perlin4.reseed(125);

    for (float x = -1.0f; x < 1.0f; x += grid_size)
    {
        for (float y = -1.0f; y < 1.0f; y += grid_size)
        {
            juce::Point<float> currentPoint(x, y);
            float r = std::sqrt(x * x + y * y);
            if (r > 1.0f)
            {
                continue;
            }

            float z = std::sqrt(std::max(0.0f, 1.0f - x * x - y * y));
            float phi = std::atan2(y, x);
            float theta = std::acos(z);

            float perlin_noise =
                (float)perlin.noise(x * 4.0f, y * 4.0f, z * 4.0f);
            float perlin_noise2 =
                (float)perlin2.noise(x * 8.0f, y * 8.0f, z * 8.0f);
            float perlin_noise3 =
                (float)perlin3.noise(x * 16.0f, y * 16.0f, z * 16.0f);
            float ridge_noise = 1.0f - std::abs(perlin_noise);
            float base_noise = 0.5f * perlin_noise + 0.25f * perlin_noise2 +
                               0.25f * perlin_noise3 +
                               std::pow(ridge_noise, 2.0f);

            float perlin_noise4 =
                (float)perlin4.noise(x * 8.0f, y * 8.0f, z * 8.0f);
            float flare_noise = std::pow(std::abs(perlin_noise4), 2.0f);
            base_noise = base_noise + 2.0f * flare_noise;

            float random_noise = 0.0f;
            float alpha = 0.0f;
            if (r >= 0.9f)
            {
                base_noise = std::min(1.0f, base_noise + 0.4f);
                alpha = 0.8f;
                random_noise = 1.0f * juce::jlimit(0.0f, 1.0f, perlin_noise);
            }
            else
            {
                float max_alpha = 0.5f;
                float min_alpha = 0.0f;
                float max_noise = 1.5f;
                float min_noise = -0.3f;
                float noise_rescaled =
                    (base_noise - min_noise) / (max_noise - min_noise);
                alpha = juce::jlimit(
                    min_alpha, max_alpha,
                    (max_alpha - min_alpha) * std::pow(noise_rescaled, 2.0f) +
                        min_alpha
                );
                float random_value = random.nextFloat();
                if (random_value < 0.05f)
                {
                    alpha = 0.8f;
                    random_noise = 10.0f * random_value;
                }
            }
            float rs = 1.0f + base_noise * 0.06f + random_noise * 0.15f;
            // Get the color based on the y position
            juce::Colour dotColour =
                c1.interpolatedWith(c2, (y + 1.0f) / 2.0f).withAlpha(alpha);

            // Revert back to cartesian coordinates
            float xc = rs * std::sin(theta) * std::cos(phi);
            float yc = rs * std::sin(theta) * std::sin(phi);
            float zc = std::sqrt(rs * rs - xc * xc - yc * yc);

            // Project the 3D points onto 2D using perspective projection
            float dist_camera_z = camera_z_offset - zc;
            float x_p = (xc * focal_length) / dist_camera_z;
            float y_p = (yc * focal_length) / dist_camera_z;

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

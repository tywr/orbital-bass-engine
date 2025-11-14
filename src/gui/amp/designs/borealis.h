#pragma once
#include "../utils/graphics.h"
#include "../utils/voronoi.h"

inline void paintIconBorealis(
    juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour c1,
    juce::Colour c2
)
{
    const int borderThickness = 2.0f;

    std::vector<juce::Path> cells = getVoronoiCells();

    auto center = bounds.getCentre();
    float maxRadius = bounds.getHeight() * 0.45f;

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

    g.setColour(c1);
    g.fillEllipse(
        center.x - maxRadius, center.y - maxRadius, maxRadius * 2, maxRadius * 2
    );

    const juce::Rectangle<float> templateBounds(0.0f, 0.0f, 1000.0f, 1000.0f);
    float scaleX = bounds.getWidth() / templateBounds.getWidth();
    float scaleY = bounds.getHeight() / templateBounds.getHeight();

    juce::AffineTransform transform =
        juce::AffineTransform::scale(scaleX, scaleY)
            .translated(bounds.getX(), bounds.getY());

    for (size_t i = 0; i < cells.size(); ++i)
    {
        float stroke_width = 0.003f * maxRadius;
        double y = getAverageY(cells[i]);
        float proportion = (float)(y / templateBounds.getHeight());
        juce::Colour currentColour = c1.interpolatedWith(c2, proportion);

        g.setColour(currentColour);
        g.fillPath(cells[i], transform);

        // g.setColour(juce::Colours::white.withAlpha(0.15f));
        // g.strokePath(cells[i], juce::PathStrokeType(1.5f), transform);

        g.setColour(juce::Colours::black);
        g.strokePath(cells[i], juce::PathStrokeType(stroke_width), transform);
    }
}

inline void paintDesignBorealis(
    juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour c1,
    juce::Colour c2
)
{
    juce::Graphics::ScopedSaveState state(g);
    auto center = bounds.getCentre();

    float maxRadius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.45f;

    juce::Path boxPath;
    boxPath.addRectangle(bounds);
    g.reduceClipRegion(boxPath);

    const int numBackgroundNoiseLayers = 20;
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

    paintIconBorealis(
        g, bounds.withSizeKeepingCentre(2 * maxRadius, bounds.getHeight()), c1,
        c2
    );
}

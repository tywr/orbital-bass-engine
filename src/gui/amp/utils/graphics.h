#pragma once

inline double getAverageY(const juce::Path& path)
{
    if (path.isEmpty())
        return 0.0;

    double totalY = 0.0;
    int pointCount = 0;

    // PathFlatteningIterator walks along the path, converting curves to line
    // segments.
    juce::PathFlatteningIterator iterator(path);

    while (iterator.next())
    {
        // For each point in the flattened version of the path...
        totalY += iterator.y1;
        pointCount++;
    }

    // Avoid division by zero for valid but non-advancing paths.
    if (pointCount == 0)
        return 0.0;

    return totalY / pointCount;
}

inline juce::Path createHexagonPath(juce::Point<float> center, float radius)
{
    juce::Path hexPath;

    // A flat-topped hexagon's vertices are at 30, 90, 150, 210, 270, 330
    // degrees
    for (int i = 0; i < 6; ++i)
    {
        // Start angle is 30 degrees (pi/6 radians) to make the top and bottom
        // flat
        float angle =
            juce::MathConstants<float>::pi / 6.0f +
            juce::MathConstants<float>::twoPi * (static_cast<float>(i) / 6.0f);

        juce::Point<float> vertex(
            center.x + radius * std::cos(angle),
            center.y + radius * std::sin(angle)
        );

        if (i == 0)
            hexPath.startNewSubPath(vertex);
        else
            hexPath.lineTo(vertex);
    }

    hexPath.closeSubPath();
    return hexPath;
}

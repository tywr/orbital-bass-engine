inline void paintIconClassic810(
    juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour c
)
{
    g.saveState();
    g.setColour(c);

    auto amp_bounds = bounds.withSizeKeepingCentre(
        bounds.getWidth() * 0.5f, bounds.getHeight()
    );
    const float w = amp_bounds.getWidth();
    const float h = amp_bounds.getHeight();

    const float lineThickness = w * 0.02f;
    const float frameInset = w * 0.08f;
    auto frameBounds = amp_bounds.reduced(frameInset);
    const float frameCornerSize = w * 0.08f;
    g.drawRoundedRectangle(frameBounds, frameCornerSize, lineThickness * 2.0f);

    auto speakerArea = frameBounds.reduced(lineThickness * 2.0f);

    const float gX = speakerArea.getX();
    const float gY = speakerArea.getY();
    const float gW = speakerArea.getWidth();
    const float gH = speakerArea.getHeight();

    // Divide the speaker area into a 2x2 grid
    const float cellW = gW / 2.0f;
    const float cellH = gH / 4.0f;

    // Define speaker radii
    const float speakerOuterRadius = juce::jmin(cellW, cellH) * 0.42f;
    const float speakerInnerRadius = speakerOuterRadius * 0.45f;

    // Loop through the 4x2 grid
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 2; ++col)
        {
            // Find the center point of the current cell
            const float cX = gX + (col * cellW) + (cellW / 2.0f);
            const float cY = gY + (row * cellH) + (cellH / 2.0f);

            g.drawEllipse(
                cX - speakerOuterRadius, cY - speakerOuterRadius,
                speakerOuterRadius * 2.0f, speakerOuterRadius * 2.0f,
                lineThickness
            );

            juce::Rectangle<float> innerSpeakerRect(
                speakerInnerRadius * 2.0f, speakerInnerRadius * 2.0f
            );
            innerSpeakerRect.setCentre(cX, cY);
            g.fillEllipse(innerSpeakerRect);
        }
    }

    const float grilleLineSpacing = speakerArea.getWidth() * 0.06f;
    for (float x = speakerArea.getX();
         x < speakerArea.getRight() + grilleLineSpacing;
         x += grilleLineSpacing)
    {
        g.drawLine(
            x, speakerArea.getY(), x, speakerArea.getBottom(),
            lineThickness * 0.5f
        );
    }
    for (float y = speakerArea.getY();
         y < speakerArea.getBottom() + grilleLineSpacing;
         y += grilleLineSpacing)
    {
        g.drawLine(
            speakerArea.getX(), y, speakerArea.getRight(), y,
            lineThickness * 0.5f
        );
    }

    g.restoreState();
}

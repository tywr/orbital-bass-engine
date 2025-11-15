#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

template <typename FloatType>
class LookupTableTransformCubic
{
  public:
    LookupTableTransformCubic() = default;

    /**
     * Initializes the lookup table with cubic spline interpolation.
     *
     * @param functionToApproximate The function to sample and approximate
     * @param minInputValue The minimum input value for the lookup table
     * @param maxInputValue The maximum input value for the lookup table
     * @param numPoints The number of points in the lookup table
     */
    void initialise(
        const std::function<FloatType(FloatType)>& functionToApproximate,
        FloatType minInputValue, FloatType maxInputValue, size_t numPoints
    )
    {
        min_value = minInputValue;
        max_value = maxInputValue;
        num_points = numPoints;

        // Allocate storage
        table.resize(numPoints);
        derivatives.resize(numPoints);

        // Calculate step size
        FloatType step = (maxInputValue - minInputValue) / (numPoints - 1);

        // Sample the function
        for (size_t i = 0; i < numPoints; ++i)
        {
            FloatType x = minInputValue + i * step;
            table[i] = functionToApproximate(x);
        }

        // Calculate derivatives using natural cubic spline approach
        // Using a simple centered difference for interior points
        // and one-sided differences at boundaries
        for (size_t i = 0; i < numPoints; ++i)
        {
            if (i == 0)
            {
                // Forward difference at start
                derivatives[i] = (table[1] - table[0]) / step;
            }
            else if (i == numPoints - 1)
            {
                // Backward difference at end
                derivatives[i] = (table[i] - table[i - 1]) / step;
            }
            else
            {
                // Centered difference for interior points
                derivatives[i] = (table[i + 1] - table[i - 1]) / (2.0f * step);
            }
        }

        // Pre-calculate reciprocal for faster division
        range = maxInputValue - minInputValue;
        inv_range = (numPoints - 1) / range;
    }

    /**
     * Processes a single sample using cubic Hermite interpolation.
     *
     * @param input The input value to transform
     * @return The interpolated output value
     */
    FloatType processSample(FloatType input) const
    {
        // Clamp input to valid range
        if (input <= min_value)
            return table[0];
        if (input >= max_value)
            return table[num_points - 1];

        // Calculate floating-point index
        FloatType float_index = (input - min_value) * inv_range;

        // Get integer and fractional parts
        size_t index = static_cast<size_t>(float_index);
        FloatType frac = float_index - index;

        // Ensure we don't go out of bounds
        if (index >= num_points - 1)
        {
            return table[num_points - 1];
        }

        // Get values and derivatives for Hermite interpolation
        FloatType y0 = table[index];
        FloatType y1 = table[index + 1];
        FloatType d0 = derivatives[index];
        FloatType d1 = derivatives[index + 1];

        // Calculate step size for this segment
        FloatType h = range / (num_points - 1);

        // Scale derivatives by step size for Hermite interpolation
        d0 *= h;
        d1 *= h;

        // Cubic Hermite interpolation
        // H(t) = (2t³ - 3t² + 1)y₀ + (t³ - 2t² + t)d₀ + (-2t³ + 3t²)y₁ + (t³
        // - t²)d₁
        FloatType t = frac;
        FloatType t2 = t * t;
        FloatType t3 = t2 * t;

        FloatType h00 = 2 * t3 - 3 * t2 + 1;    // (1 + 2t)(1-t)²
        FloatType h10 = t3 - 2 * t2 + t;        // t(1-t)²
        FloatType h01 = -2 * t3 + 3 * t2;       // t²(3-2t)
        FloatType h11 = t3 - t2;                // t²(t-1)

        return h00 * y0 + h10 * d0 + h01 * y1 + h11 * d1;
    }

  private:
    std::vector<FloatType> table;
    std::vector<FloatType> derivatives;
    FloatType min_value{0};
    FloatType max_value{0};
    FloatType range{0};
    FloatType inv_range{0};
    size_t num_points{0};
};

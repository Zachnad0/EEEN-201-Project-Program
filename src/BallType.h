#pragma once

#include <TCS3200.h>
#include <stdint.h>
#include <type_traits>
#define pow2(x) (x) * (x)

// VALID BALL PROPERTIES:
// TT: yellow or white | low mass  | unknown stress
// GP: white,          | low mass  | unknown stress
// SQ: black,          | high mass | unknown stress

namespace eeen201
{
    inline float NormalPDF(float x, float mean, float variance)
    {
        variance = abs(variance);
        return (float)(exp(-pow(x - mean, 2) / (2 * variance)) / sqrt(2 * PI * variance));
    }

    class BallType
    {
    public:
        const RGBColor color;
        const uint32_t massMean, massVariance, stressMean, stressVariance;

        BallType(RGBColor color, uint32_t massMean, uint32_t massVariance, uint32_t stressMean, uint32_t stressVariance)
            : color(color), massMean(massMean), massVariance(massVariance), stressMean(stressMean), stressVariance(stressVariance)
        {
        }

        /// @brief Evaluates how well the given properties match this ball type
        /// @return Positive floating-point error value. The smaller, the closer of a match.
        float ComputeMatchError(RGBColor mColor, uint32_t mMass, uint32_t mStress) const
        {
            constexpr float COLOR_WEIGHT = 1, MASS_WEIGHT = 1, STRESS_WEIGHT = 1;
            float error = 0;

            // Color
            // TODO test and tweak color error
            int16_t rDiff = mColor.red - (int16_t)color.red;
            int16_t gDiff = mColor.green - (int16_t)color.green;
            int16_t bDiff = mColor.blue - (int16_t)color.blue;
            error += COLOR_WEIGHT * (pow2(rDiff) + pow2(gDiff) + pow2(bDiff)) / 3.0f;

            // Mass and stress be using normal distribution now!
            float massScore = NormalPDF(mMass, massMean, massVariance); // Higher is closer
            float stressScore = NormalPDF(mStress, stressMean, stressVariance);
            error += MASS_WEIGHT * (1 - massScore) + STRESS_WEIGHT * (1 - stressScore);

            return error;
        }
    };
}

#undef pow2

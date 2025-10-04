#pragma once

#include <TCS3200.h>
#include <stdint.h>
#define pow2(x) (x) * (x)

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
        const char *name;
        const RGBColor color;
        const uint32_t massMean, massVariance, stressMean, stressVariance;
        const bool isValid;

        BallType(const char *name, RGBColor color, uint32_t massMean, uint32_t massVariance, uint32_t stressMean, uint32_t stressVariance, bool isValid = true)
            : name(name), color(color), massMean(massMean), massVariance(massVariance), stressMean(stressMean), stressVariance(stressVariance), isValid(isValid)
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
            int32_t rDiff = mColor.red - (int32_t)color.red;
            int32_t gDiff = mColor.green - (int32_t)color.green;
            int32_t bDiff = mColor.blue - (int32_t)color.blue;
            float colorError = COLOR_WEIGHT * (pow2(rDiff) + pow2(gDiff) + pow2(bDiff)) / 3.0f;
            // Serial.print("Color Error: ");
            // Serial.println(colorError);
            error += colorError;

            // Mass and stress be using normal distribution now!
            float massScore = NormalPDF(mMass, massMean, massVariance); // Higher is closer
            // Serial.print("Mass Score: ");
            // Serial.println(massScore);
            float stressScore = NormalPDF(mStress, stressMean, stressVariance);
            // Serial.print("Stress Score: ");
            // Serial.println(stressScore);
            error += MASS_WEIGHT * (1 - massScore) + STRESS_WEIGHT * (1 - stressScore);

            return error;
        }
    };
}

#undef pow2

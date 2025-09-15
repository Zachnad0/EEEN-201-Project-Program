#pragma once

#include <stdint.h>
#include <TCS3200.h>

// TODO define these
#define PIN_SERVO_DROP 0
#define PIN_SERVO_SORT 0
#define PIN_COLSEN_S0 0
#define PIN_COLSEN_S1 0
#define PIN_COLSEN_S2 0
#define PIN_COLSEN_S3 0
#define PIN_COLSEN_OUT 0

// VALID BALL PROPERTIES:
// TT: yellow or white | low mass  | high bounce interval
// GP: white,          | low mass  | low bounce interval
// SQ: black,          | high mass | low-ish range of bounce intervals

namespace eeen201
{
    class BallType
    {
    public:
        const RGBColor color;
        const uint32_t minMass, maxMass, minBounceInterval, maxBounceInterval;

        BallType(RGBColor color, uint32_t minMass, uint32_t maxMass, uint32_t minBounceInterval, uint32_t maxBounceInterval)
            : color(color), minMass(minMass), maxMass(maxMass), minBounceInterval(minBounceInterval), maxBounceInterval(maxBounceInterval)
        {
        }

#define pow2(x) (x) * (x)
        /// @brief Evaluates how well the given properties match this ball type
        /// @return Positive floating-point error value. The smaller, the closer of a match.
        float ComputeMatchError(RGBColor mColor, uint32_t mass, uint32_t bounceInterval) const
        {
            constexpr float COLOR_WEIGHT = 1, MASS_WEIGHT = 1, BI_WEIGHT = 1;
            float error = 0;

            // Color
            // TODO test and tweak color error
            int16_t rDiff = mColor.red - (int16_t)color.red;
            int16_t gDiff = mColor.green - (int16_t)color.green;
            int16_t bDiff = mColor.blue - (int16_t)color.blue;
            error += COLOR_WEIGHT * (pow2(rDiff) + pow2(gDiff) + pow2(bDiff)) / 3.0;

            // Mass
            if (mass < minMass)
            {
                error += pow2(minMass - mass) * MASS_WEIGHT;
            }
            else if (mass > maxMass)
            {
                error += pow2(mass - maxMass) * MASS_WEIGHT;
            }
            // No addition to error if mass is within range

            // Bounce Interval
            if (bounceInterval < minBounceInterval)
            {
                error += pow2(minBounceInterval - bounceInterval) * BI_WEIGHT;
            }
            else if (bounceInterval > maxBounceInterval)
            {
                error += pow2(bounceInterval - maxBounceInterval) * BI_WEIGHT;
            }
            // No addition to error if bounce interval is within range

            return error;
        }
#undef pow2
    };
}

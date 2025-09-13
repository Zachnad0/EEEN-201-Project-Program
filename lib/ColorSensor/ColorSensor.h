#pragma once

#include <Arduino.h>
#include <ProjDefs.h>
#include <stdint.h>
#include <TCS3200.h>

namespace eeen201
{
    /// @brief Singleton class for interfacing with the TCS3200 DFROBOT sensor.
    class ColorSensor
    {
    private:
        static TCS3200 *colorSensor;

    public:
        ColorSensor(ColorSensor &) = delete;
        ColorSensor &operator=(ColorSensor &) = delete;
        ColorSensor() = delete;

        static void EnsureInit()
        {
            if (colorSensor != nullptr)
                return;

            colorSensor = new TCS3200(PIN_COLSEN_S0, PIN_COLSEN_S1, PIN_COLSEN_S2, PIN_COLSEN_S3, PIN_COLSEN_OUT);
            colorSensor->begin();
            colorSensor->frequency_scaling(TCS3200_OFREQ_20P);
            // TODO figure out sensor calibration.
        }
    };

    TCS3200 *ColorSensor::colorSensor = nullptr;
}
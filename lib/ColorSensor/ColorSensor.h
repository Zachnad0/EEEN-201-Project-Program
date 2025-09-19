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
        ColorSensor(const ColorSensor &) = delete;
        ColorSensor &operator=(const ColorSensor &) = delete;
        ColorSensor() = delete;

        static void EnsureInit()
        {
            if (colorSensor != nullptr)
                return;

            colorSensor = new TCS3200(PIN_COLSEN_S0, PIN_COLSEN_S1, PIN_COLSEN_S2, PIN_COLSEN_S3, PIN_COLSEN_OUT);
            colorSensor->begin();
            colorSensor->frequency_scaling(TCS3200_OFREQ_20P);
            colorSensor->integration_time(100);
            // TODO figure out if sensor calibration is practically necessary (technically optional)

            // // Calibrate the sensor for accurate color readings
            // Serial.println("Calibrating...");
            // // Calibrate the sensor for light surface (white calibration)
            // Serial.println("Please face the sensor to white surface.");
            // delay(2000);
            // colorSensor->calibrate_light();

            // // Calibrate the sensor for dark surface (black calibration)
            // Serial.println("Please face the sensor to dark surface.");
            // delay(2000);
            // colorSensor->calibrate_dark();

            // // Complete calibration and enable calibrated readings
            // Serial.println("Done calibrating!");
            // colorSensor->calibrate();

            // Add a delay to allow time for the serial output to be read
            delay(1000);
        }

        static inline RGBColor SampleColor()
        {
            EnsureInit();
            // colorSensor->loop();
            return colorSensor->read_rgb_color();
        }
    };

    TCS3200 *ColorSensor::colorSensor = nullptr;
}
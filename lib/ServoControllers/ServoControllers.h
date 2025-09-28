#pragma once
#include <Arduino.h>
#include <stdint.h>

#define MAX_ANGLE_PULSE_us 2000
#define MIN_ANGLE_PULSE_us 1000
#define MIN_UPDATE_INTERVAL_ms 10

namespace eeen201
{
    class ServoBase
    {
    protected:
        uint8_t controlPin;
        uint32_t pulseLength_us = 0;
        uint32_t lastUpdateTime_ms = 0;

    public:
        ServoBase(ServoBase &) = delete;
        ServoBase &operator=(ServoBase &) = delete;

        ServoBase(uint8_t contPin) : controlPin{contPin}
        {
            pinMode(contPin, OUTPUT);
        }

        void Update()
        {
            if (millis() - lastUpdateTime_ms < MIN_UPDATE_INTERVAL_ms)
                return;
            digitalWrite(controlPin, HIGH);
            delayMicroseconds(pulseLength_us);
            digitalWrite(controlPin, LOW);
            lastUpdateTime_ms = millis();
        }
    };

    class AngleServo : public ServoBase
    {
    public:
        /// @brief Sets motor angle to given value. Update() to be called to move motor.
        /// @param angle Must be between 0 and 90
        void SetAngle(float angle)
        {
            // Translate angle to pulseLength for HD-3001HB
            angle = constrain(angle, 0, 90);
            pulseLength_us = (uint32_t)((angle / 90.0f) * (MAX_ANGLE_PULSE_us - MIN_ANGLE_PULSE_us)) + MIN_ANGLE_PULSE_us;
        }

        AngleServo(uint8_t contPin) : ServoBase(contPin)
        {
            pulseLength_us = (MAX_ANGLE_PULSE_us + MIN_ANGLE_PULSE_us) / 2;
        }
    };

    // class ContServo : public ServoBase // If it ain't necessary, don't bother
    // {
    // public:
    //     void SetSpeed(uint16_t speed)
    //     {
    //         // Translate speed to pulseLength
    //         // For AR-3606HB
    //     }
    // };
}

#undef MAX_ANGLE_PULSE_us
#undef MIN_ANGLE_PULSE_us
#undef MIN_UPDATE_INTERVAL_ms
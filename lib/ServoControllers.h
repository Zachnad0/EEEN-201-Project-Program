#pragma once
#include <Arduino.h>
#include <stdint.h>

namespace eeen201
{
    class ServoBase
    {
    protected:
        uint8_t controlPin, pulseLength_us = 0;

    public:
        ServoBase(ServoBase &) = delete;
        bool operator=(ServoBase &) = delete;
        virtual inline void Stop() = 0;
        virtual inline void Start() = 0;

        ServoBase(uint8_t contPin) : controlPin{contPin}
        {
            if (!digitalPinHasPWM(contPin))
            {
                // This should not happen
                return;
            }
            pinMode(contPin, OUTPUT);
        }

        void Update()
        {
            delayMicroseconds(5000);
            digitalWrite(controlPin, HIGH);
            delayMicroseconds(pulseLength_us);
            digitalWrite(controlPin, LOW);
        }
    };

    class AngleServo : public ServoBase
    {
    public:
        /// @brief Sets motor angle to given value. Update() to be called to send move motor.
        /// @param angle Must be between 0 and 90
        void SetAngle(float angle)
        {
            // Translate angle to pulseLength for HD-3001HB
            angle = constrain(angle, 0, 90);
            pulseLength_us = (angle / 90 + 1) * 1000;
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
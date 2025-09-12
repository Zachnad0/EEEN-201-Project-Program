#pragma once
#include <Arduino.h>
#include <stdint.h>

#define _mostAngleLen_us 2000
#define _leastAngleLen_us 1000

namespace eeen201
{
    class ServoBase
    {
    protected:
        uint8_t controlPin;
        uint32_t pulseLength_us = 0;

    public:
        ServoBase(ServoBase &) = delete;
        ServoBase& operator=(ServoBase &) = delete;

        ServoBase(uint8_t contPin) : controlPin{contPin}
        {
            pinMode(contPin, OUTPUT);
        }

        void Update()
        {
            delay(10);
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
            pulseLength_us = (uint32_t)((angle / 90.0f) * (_mostAngleLen_us - _leastAngleLen_us)) + _leastAngleLen_us;
        }

        AngleServo(uint8_t contPin) : ServoBase(contPin)
        {
            pulseLength_us = (_mostAngleLen_us - _leastAngleLen_us) / 2 + _leastAngleLen_us;
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

#undef _mostAngleLen_us
#undef _leastAngleLen_us
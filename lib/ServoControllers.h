#pragma once
#include <Arduino.h>
#include <stdint.h>

namespace eeen201
{
    class ServoBase
    {
    protected:
        uint8_t controlPin;

    public:
        virtual inline void Stop() = 0;
        virtual inline void Start() = 0;

        ServoBase(uint8_t contPin) : controlPin{contPin}
        {
            pinMode(contPin, OUTPUT);
            digitalWrite(contPin, LOW);
        }
    };

    class AngleServo : public ServoBase
    {
    public:
        void SetAngle(uint16_t angle);
    };

    class ContServo : public ServoBase
    {
    public:
        void SetSpeed(uint16_t speed);
    };
}
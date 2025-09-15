#include <ProjDefs.h>
#include <Arduino.h>
#include <ServoControllers.h>
#include <LoadCell.h>
#include <ColorSensor.h>

eeen201::AngleServo *dropServo, *sortServo;

/// @brief Measures the interval between ball bounces on load cell.
/// @return Bounce interval in milliseconds, else 0 if failed/timeout.
uint32_t TestBounceInterval()
{
    constexpr int32_t MAXTIME_FIRST_ms = 0, MAXTIME_SECOND_ms = 0, MININTERVAL_ms = 0, VAL_THRESHOLD = 0;
    // Measure time between first two times a reading exceeding threshold is detected

    eeen201::LoadCell::Recalibrate();
    uint32_t lastTime = millis();
    while (eeen201::LoadCell::ReadNextVal() < VAL_THRESHOLD)
    {
        if ((millis() - lastTime) > MAXTIME_FIRST_ms)
            return 0;
    }
    lastTime = millis();
    delay(MININTERVAL_ms);
    while (eeen201::LoadCell::ReadNextVal() < VAL_THRESHOLD)
    {
        if ((millis() - lastTime) > MAXTIME_SECOND_ms)
            return 0;
    }

    uint32_t interval = millis() - lastTime;
    return interval;
}

void setup()
{
    Serial.begin(115200);
    // Init classes
    eeen201::LoadCell::EnsureInit();
    // eeen201::ColorSensor::EnsureInit();
    dropServo = new eeen201::AngleServo(PIN_SERVO_DROP);
    sortServo = new eeen201::AngleServo(PIN_SERVO_SORT);
}

void loop()
{
    // TODO test TestBounceInterval function
}
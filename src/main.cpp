#include <ProjDefs.h>
#include <Arduino.h>
#include <ServoControllers.h>
#include <LoadCell.h>
#include <ColorSensor.h>

eeen201::AngleServo *dropServo, *sortServo;

uint32_t TestBounceInterval()
{
    constexpr uint32_t MAXTIME_ms = 0, DOWNTIME_ms = 0, VAL_THRESHOLD = 0;
    // Measure time between first two times a constant threshold reading is detected.

    // TODO complete method, then test
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
    // Set up for testing
}
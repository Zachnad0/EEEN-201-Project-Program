#include <Arduino.h>
#include <ServoControllers.h>
#include <LoadCell.h>

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
    eeen201::LoadCell::EnsureInit();
}

void loop()
{
    // Set up for testing
}
#include <ProjDefs.h>
#include <Arduino.h>
#include <ServoControllers.h>
#include <LoadCell.h>
#include <ColorSensor.h>
#include <BallType.h>

eeen201::AngleServo *dropServo, *sortServo;
const eeen201::BallType BALL_DEFS[] =
    {
        // Format: {r,g,b}, minMass, maxMass, minBI, maxBI
        {{1, 1, 1}, 2, 2, 3, 3}, 
};

constexpr size_t BALL_DEF_LEN = sizeof(BALL_DEFS) / sizeof(eeen201::BallType);

const eeen201::BallType* FindClosestType(RGBColor color, uint32_t mass, uint32_t bounceInterval)
{
    // Determines the type with the lowest error as the best match
    const eeen201::BallType *closestType = nullptr;
    float closestError = INFINITY;
    for (size_t i = 0; i < BALL_DEF_LEN; i++)
    {
        const eeen201::BallType *currType = BALL_DEFS + i;
        float currError = currType->ComputeMatchError(color, mass, bounceInterval);
        if (currError < closestError)
        {
            closestError = currError;
            closestType = currType;
        }
    }

    return closestType;
}

/// @brief Measures the interval between ball bounces on load cell.
/// @return Bounce interval in milliseconds, else 0 if failed/timeout.
uint32_t TestBounceInterval()
{
    constexpr int32_t MAXTIME_FIRST_ms = 5000, MAXTIME_SECOND_ms = 3000, MININTERVAL_ms = 100, VAL_THRESHOLD = 10000;
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
    delay(250);

    // TODO test TestBounceInterval function
    for (uint8_t i = 3; i != 0; i--)
    {
        Serial.print("Testing in ");
        Serial.print((int)i);
        Serial.println("...");
        delay(1000);
    }
    Serial.println("Now");
    uint32_t result = TestBounceInterval();
    Serial.print("Done, result = ");
    Serial.println(result);
}

void loop()
{
    delay(1000);
}
#include <Arduino.h>
#include <ProjDefs.h>

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

const eeen201::BallType *FindClosestType(RGBColor color, uint32_t mass, uint32_t bounceInterval)
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
    constexpr int32_t MAXTIME_FIRST_ms = 50000, MAXTIME_SECOND_ms = 30000, MININTERVAL_ms = 100, VAL_THRESHOLD = 10000;
    // Measure time between first two times a reading exceeding threshold is detected

    eeen201::LoadCell::Recalibrate();
    Serial.println(eeen201::LoadCell::ReadNextVal());
    uint32_t lastTime = millis();
    while (eeen201::LoadCell::ReadNextVal() < VAL_THRESHOLD)
    {
        if ((millis() - lastTime) > MAXTIME_FIRST_ms)
            return 0;
    }
    Serial.println("First hit!");
    lastTime = millis();
    delay(MININTERVAL_ms);
    while (eeen201::LoadCell::ReadNextVal() < VAL_THRESHOLD)
    {
        if ((millis() - lastTime) > MAXTIME_SECOND_ms)
            return 0;
    }

    Serial.println("Second hit!");
    uint32_t interval = millis() - lastTime;
    return interval;
}

void setup()
{
    Serial.begin(115200);
    // Init classes
    eeen201::LoadCell::EnsureInit();
    // eeen201::ColorSensor::EnsureInit();
    // dropServo = new eeen201::AngleServo(PIN_SERVO_DROP);
    // sortServo = new eeen201::AngleServo(PIN_SERVO_SORT);
    delay(250);

    // TODO test TestBounceInterval function
    // for (uint8_t i = 3; i != 0; i--)
    // {
    //     Serial.print("Testing in ");
    //     Serial.print((int)i);
    //     Serial.println("...");
    //     delay(1000);
    // }

    // Testing of color sensor
}

constexpr uint8_t SAMPLE_LEN = 10;
uint32_t samples[SAMPLE_LEN];

void loop()
{
    // RGBColor currColor = eeen201::ColorSensor::SampleColor();
    // Serial.println((int)currColor.red);
    // Serial.println((int)currColor.green);
    // Serial.println((int)currColor.blue);
    // Serial.println("==========================================================");

    // if (millis() - lasttime > 3000)
    // {
    //     eeen201::LoadCell::Recalibrate();
    //     lasttime = millis();
    //     Serial.println("recal");
    // }

    // uint32_t sampleSum = 0;
    for (uint8_t i = 0; i < SAMPLE_LEN; i++)
    {
        delay(5000);
        Serial.println("Now");
        uint32_t result = TestBounceInterval();
        Serial.print("Done, result = ");
        Serial.println(result);
        samples[i] = result;
        // sampleSum += result;
    }

    Serial.println("CSV:");
    for (uint8_t i = 0; i < SAMPLE_LEN; i++)
    {
        Serial.print(samples[i]);
        Serial.print(";");
    }

    // Calc and print avg, stddev
    // float mean = sampleSum / (float)SAMPLE_LEN;
    // float var = 0;
    // for (uint8_t i = 0; i < SAMPLE_LEN; i++)
    // {
    //     float x = samples[i] - mean;
    //     var += x * x;
    // }
    // var /= SAMPLE_LEN;

    // Serial.print("AVG: ");
    // Serial.println(mean);
    // Serial.print("VAR: ");
    // Serial.println(var);
}

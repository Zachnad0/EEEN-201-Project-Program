#include <Arduino.h>
#include <StandardCplusplus.h>
#include <vector>
#include <algorithm>

#include <ProjDefs.h>
#include <ServoControllers.h>
#include <LoadCell.h>
#include <ColorSensor.h>
#include <BallType.h>

eeen201::AngleServo *strainServo, *sortServo;
const eeen201::BallType BALL_DEFS[] =
    {
        // Format: {r,g,b}, massMean, massVariance, stressMean, stressVariance
        {{1, 1, 1}, 2, 2, 3, 3},
};

constexpr size_t BALL_DEF_LEN = sizeof(BALL_DEFS) / sizeof(eeen201::BallType);

const eeen201::BallType *FindClosestType(RGBColor color, uint32_t mass, uint32_t stress)
{
    // Determines the type with the lowest error as the best match
    const eeen201::BallType *closestType = nullptr;
    float closestError = INFINITY;
    for (size_t i = 0; i < BALL_DEF_LEN; i++)
    {
        const eeen201::BallType *currType = BALL_DEFS + i;
        float currError = currType->ComputeMatchError(color, mass, stress);
        if (currError < closestError)
        {
            closestError = currError;
            closestType = currType;
        }
    }

    return closestType;
}

/*
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
*/

/// @brief (Run after mass test) Performs stress test on ball by rotating servo to apply a controlled strain, then uses load cell to measure stress.
/// @return Load cell reading when strained
int32_t StressTest()
{
    constexpr size_t SAMPLE_N = 11;
    constexpr uint32_t DELAY_PRESS_ms = 3000, DELAY_RELEASE_ms = DELAY_PRESS_ms;
    constexpr float ANGLE_STANDBY = 90, ANGLE_PRESSING = 0;

    // Assuming servo already in standby state, apply strain
    eeen201::LoadCell::Recalibrate();
    strainServo->SetAngle(ANGLE_PRESSING);
    uint32_t moveStart_ms = millis();
    while (millis() - moveStart_ms < DELAY_PRESS_ms)
        strainServo->Update();
    delay(100);

    // Take median of 11 samples
    std::vector<int32_t> samples{SAMPLE_N};
    for (uint16_t n = 0; n < SAMPLE_N; n++)
    {
        samples.push_back(eeen201::LoadCell::ReadNextVal());
        delay(10);
    }
    std::sort(samples.begin(), samples.end());
    int32_t medianReading = samples.at(SAMPLE_N / 2);

    // Release strain and return to standby position
    strainServo->SetAngle(ANGLE_STANDBY);
    moveStart_ms = millis();
    while (millis() - moveStart_ms < DELAY_RELEASE_ms)
        strainServo->Update();
    delay(100);

    return medianReading;
}

void setup()
{
    Serial.begin(115200);
    // Init classes
    // eeen201::LoadCell::EnsureInit();
    eeen201::ColorSensor::EnsureInit();
    // strainServo = new eeen201::AngleServo(PIN_SERVO_STRAIN);
    // sortServo = new eeen201::AngleServo(PIN_SERVO_SORT);
    delay(250);

    // // TODO test StressTest
    // for (uint8_t i = 3; i != 0; i--)
    // {
    //     Serial.print("Testing in ");
    //     Serial.print((int)i);
    //     Serial.println("...");
    //     delay(1000);
    // }
    // int32_t reading = StressTest();
    // Serial.print("Reading: ");
    // Serial.println(reading);
}

constexpr uint8_t SAMPLE_LEN = 10;
int32_t samples[SAMPLE_LEN];

void loop()
{
    RGBColor currColor = eeen201::ColorSensor::SampleColor();
    Serial.print("#");
    Serial.print(currColor.red, HEX);
    Serial.print(currColor.green, HEX);
    Serial.println(currColor.blue, HEX);
    // Serial.println("==========================================================");
    delay(1000);

    // // TODO Acquire readings of stress for each ball
    // for (uint8_t k = 0; k < SAMPLE_LEN; k++)
    // {
    //     delay(5000);
    //     for (uint8_t i = 3; i != 0; i--)
    //     {
    //         Serial.print("Testing in ");
    //         Serial.print((int)i);
    //         Serial.println("...");
    //         delay(1000);
    //     }
    // int32_t reading = StressTest();
    //     samples[k] = reading;
    //     Serial.print("Reading: ");
    //     Serial.println(reading);
    // }

    // Serial.println("CSV:");
    // for (uint8_t i = 0; i < SAMPLE_LEN; i++)
    // {
    //     Serial.print(samples[i]);
    //     if (i != SAMPLE_LEN - 1)
    //         Serial.print(";");
    // }
}

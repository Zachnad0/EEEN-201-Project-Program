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
std::vector<const eeen201::BallType *> processedBallTypes;

// VALID BALL PROPERTIES:
// TT: yellow or white | low mass  | unknown stress
// GP: yellow,         | low mass  | unknown stress
// SQ: black,          | high mass | unknown stress
// INVALID BALLS:
// TT: black
// GP: black, white
// SQ: white, yellow
const eeen201::BallType BALL_DEFS[] =
    {
        // Format: {r,g,b}, massMean, massVariance, stressMean, stressVariance, isValid (default true)
        // Valids:
        {"TT - W", {1, 1, 1}, 2, 2, 3, 3},
        {"TT - Y", {1, 1, 1}, 2, 2, 3, 3}, // Should have same physical properties as prev.
        {"GP - Y", {1, 1, 1}, 2, 2, 3, 3},
        {"SQ - Y", {1, 1, 1}, 2, 2, 3, 3},  // Least bouncy SQ
        {"SQ - YY", {1, 1, 1}, 2, 2, 3, 3}, // All SQs should have same color
        {"SQ - R", {1, 1, 1}, 2, 2, 3, 3},
        {"SQ - B", {1, 1, 1}, 2, 2, 3, 3}, // Most bouncy SQ

        // Invalids: (Alternatively just check if error is above some threshold?)
        {"TT - B", {1, 1, 1}, 2, 2, 3, 3},
        {"GP - B", {1, 1, 1}, 2, 2, 3, 3},
        {"GP - W", {1, 1, 1}, 2, 2, 3, 3},
        {"SQ - Y", {1, 1, 1}, 2, 2, 3, 3},
        {"SQ - W", {1, 1, 1}, 2, 2, 3, 3}, // Maybe same phys. props. as prev? (crusty paint?)
};

constexpr size_t BALL_DEF_LEN = sizeof(BALL_DEFS) / sizeof(eeen201::BallType);

const eeen201::BallType *FindClosestType(RGBColor color, int32_t mass, int32_t stress, float &minError) // TODO test method
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

    minError = closestError;
    return closestType;
}

void MoveServoToAngleForTime(eeen201::AngleServo *servo, float angle, uint32_t time_ms)
{
    servo->SetAngle(angle);
    uint32_t moveStart_ms = millis();
    while (millis() - moveStart_ms < time_ms)
        servo->Update();
}

int32_t MedianLoadCellReading(const uint16_t sampleCount)
{
    std::vector<int32_t> samples{sampleCount};
    for (uint16_t n = 0; n < sampleCount; n++)
    {
        int32_t k = eeen201::LoadCell::ReadNextVal();
        if (k == -999999) // Invalid readings are retried
        {
            Serial.println("Invalid reading in MedianLoadCellReading!!!");
            n--;
            continue;
        }
        samples.push_back(k);
        delay(10);
    }
    std::sort(samples.begin(), samples.end());
    return samples.at(sampleCount / 2);
}

/// @brief Waits then reads mass of ball on load cell.
/// @return Load cell reading at rest
int32_t MassTest() // TODO test method
{
    constexpr uint32_t DELAY_INITIAL_ms = 3000, SAMPLE_N = 11;

    eeen201::LoadCell::Recalibrate();
    // Serial.println("Recalibrated.\nMass test in 3 seconds..."); // DEBUG
    delay(DELAY_INITIAL_ms);
    int32_t medianReading = MedianLoadCellReading(SAMPLE_N);

    return medianReading;
}

/// @brief (Run after mass test) Performs stress test on ball by rotating servo to apply a controlled strain, then uses load cell to measure stress.
/// @return Load cell reading when strained
int32_t StressTest() // TODO test method
{
    constexpr size_t SAMPLE_N = 11;
    constexpr uint32_t DELAY_PRESS_ms = 3000, DELAY_RELEASE_ms = DELAY_PRESS_ms;
    constexpr float ANGLE_STANDBY = 90, ANGLE_PRESSING = 0;

    // Assuming servo already in standby state, apply strain
    eeen201::LoadCell::Recalibrate();
    MoveServoToAngleForTime(strainServo, ANGLE_PRESSING, DELAY_PRESS_ms);
    delay(100);

    // Take median of 11 samples
    int32_t medianReading = MedianLoadCellReading(SAMPLE_N);

    // Release strain and return to standby position
    MoveServoToAngleForTime(strainServo, ANGLE_STANDBY, DELAY_RELEASE_ms);
    delay(100);

    return medianReading;
}

/// @brief Utilizes color sensor to take multiple color readings then average the channels.
/// @return Averaged colors of readings
RGBColor ColorTest() // TODO test method
{
    constexpr uint32_t DELAY_INITIAL_ms = 1000, SAMPLE_N = 11;

    delay(DELAY_INITIAL_ms);
    uint16_t sumRed = 0, sumGreen = 0, sumBlue = 0;
    for (uint16_t n = 0; n < SAMPLE_N; n++)
    {
        RGBColor currColor = eeen201::ColorSensor::SampleColor();
        sumRed += currColor.red;
        sumGreen += currColor.green;
        sumBlue += currColor.blue;
        delay(10);
    }

    uint8_t avgRed = (uint8_t)(sumRed / SAMPLE_N);
    uint8_t avgGreen = (uint8_t)(sumGreen / SAMPLE_N);
    uint8_t avgBlue = (uint8_t)(sumBlue / SAMPLE_N);

    return {avgRed, avgGreen, avgBlue};
}

/// @brief Rotates the servo one way 45 degrees if valid, else in opposite direction if ball is defect.
void DoSortBall(const eeen201::BallType *ballType) // TODO test method
{
    constexpr uint32_t DELAY_INITIAL_ms = 1000, DELAY_MOVE_OUT_ms = 3000, DELAY_SETTLE_ms = 2000, DELAY_MOVE_IN_ms = DELAY_MOVE_OUT_ms;
    constexpr float ANGLE_VALID = 0, ANGLE_DEFECT = 90, ANGLE_DEFAULT = 45;

    delay(DELAY_INITIAL_ms);

    // Rotate chamber out
    MoveServoToAngleForTime(sortServo, ballType->isValid ? ANGLE_VALID : ANGLE_DEFECT, DELAY_MOVE_OUT_ms);
    delay(DELAY_SETTLE_ms); // Give time for ball to roll from chamber

    // Rotate chamber back in
    MoveServoToAngleForTime(sortServo, ANGLE_DEFAULT, DELAY_MOVE_IN_ms);
}

/// @brief Re-orients servos back to their specified default angles, over a predetermined time.
void ResetServoPositions() // TODO test method
{
    constexpr uint32_t DELAY_ORIENTATION = 3000;
    constexpr float ANGLE_DEFAULT_STRAIN = 90, ANGLE_DEFAULT_SORT = 45;

    uint32_t moveStart_ms = millis();
    sortServo->SetAngle(ANGLE_DEFAULT_SORT);
    strainServo->SetAngle(ANGLE_DEFAULT_STRAIN);
    while (millis() - moveStart_ms < DELAY_ORIENTATION)
    {
        sortServo->Update();
        strainServo->Update();
    }
}

/// @brief Literally the full sequence to be run repeatedly in the main loop
void FullBallTestSequence()
{
    Serial.println("FEED BALL NOW");

    int32_t mMass = MassTest();
    Serial.print("Mass: ");
    Serial.println(mMass);

    RGBColor mColor = ColorTest();
    String cSt = String{"Color: ("} + (int)mColor.red + ", " + (int)mColor.green + ", " + (int)mColor.blue + ")";
    Serial.println(cSt);

    int32_t mStress = StressTest();
    Serial.print("Stress: ");
    Serial.println(mStress);

    float minError;
    const eeen201::BallType *closestType = FindClosestType(mColor, mMass, mStress, minError);
    Serial.print("Closest type: ");
    Serial.println(closestType->name);
    Serial.print("Error: ");
    Serial.println(minError);
    processedBallTypes.push_back(closestType);
}

void setup()
{
    Serial.begin(115200);
    // Init classes
    eeen201::LoadCell::EnsureInit();
    eeen201::ColorSensor::EnsureInit();
    strainServo = new eeen201::AngleServo(PIN_SERVO_STRAIN);
    sortServo = new eeen201::AngleServo(PIN_SERVO_SORT);
    delay(250);
    ResetServoPositions();

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
    // RGBColor currColor = ColorTest();
    // Serial.print(currColor.red, 10);
    // Serial.print(", ");
    // Serial.print(currColor.green, 10);
    // Serial.print(", ");
    // Serial.println(currColor.blue, 10);
    // delay(1000);

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

    // // Final sequence
    // FullBallTestSequence();
    // Serial.println("\n");
    // delay(1000);
}

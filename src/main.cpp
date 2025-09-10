#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NAU7802.h>
#include "../lib/ServoControllers.h"

Adafruit_NAU7802 nau;

void setup()
{
    // pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
    nau.begin();
    delay(3000);

    nau.setGain(NAU7802_GAIN_128);
    while (!nau.calibrate(NAU7802_CALMOD_INTERNAL))
    {
        Serial.println("Failed to calibrate internal offset, retrying!");
        delay(1000);
    }
    Serial.println("Calibrated internal offset");
    while (!nau.calibrate(NAU7802_CALMOD_OFFSET))
    {
        Serial.println("Failed to calibrate system offset, retrying!");
        delay(1000);
    }
    Serial.println("Calibrated system offset");

    while (nau.available())
        nau.read();
}

void loop()
{
    // digitalWrite(LED_BUILTIN, HIGH);
    // delay(300);
    // digitalWrite(LED_BUILTIN, LOW);
    // delay(100);
    while (!nau.available())
    {
        delay(1);
    }
    int32_t val = nau.read();
    Serial.print("Read ");
    Serial.println(val);
}
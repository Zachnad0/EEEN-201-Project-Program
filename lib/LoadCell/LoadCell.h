#pragma once

#include <Adafruit_NAU7802.h>
#include <stdint.h>

namespace eeen201
{
    /// @brief Singleton class to reference THE load cell
    class LoadCell
    {
    private:
        static Adafruit_NAU7802 *loadCell;
        // static int32_t ValToGrams(int32_t val)
        // {
        //     // Hmm, may not be necessary at all.
        // }

    public:
        LoadCell(LoadCell &) = delete;
        LoadCell &operator=(LoadCell &) = delete;
        LoadCell() = delete;

        static inline void EnsureInit()
        {
            if (loadCell != nullptr)
                return;

            loadCell = new Adafruit_NAU7802();
            loadCell->begin();
            loadCell->setRate(NAU7802_RATE_320SPS);
            loadCell->setGain(NAU7802_GAIN_32);
            Recalibrate();
            while (loadCell->available())
                loadCell->read();
        }

        /// @brief Waits for next reading to be taken.
        /// @return Value that's been read. Else -999999 if nothing found.
        static int32_t ReadNextVal()
        {
            EnsureInit();

            // Only try 255 times
            for (uint8_t i = 1; !loadCell->available(); i++)
            {
                delayMicroseconds(1000000 / 320);
                if (i == 0)
                    return -999999;
            }

            int32_t val = loadCell->read();
            return val;
        }

        // static inline int32_t ReadNextGrams() { return ValToGrams(ReadNextVal()); }

        static void Recalibrate()
        {
            EnsureInit();

            while (!loadCell->calibrate(NAU7802_CALMOD_INTERNAL))
                delay(1);
            while (!loadCell->calibrate(NAU7802_CALMOD_OFFSET))
                delay(1);
        }
    };

    Adafruit_NAU7802 *LoadCell::loadCell = nullptr;
};
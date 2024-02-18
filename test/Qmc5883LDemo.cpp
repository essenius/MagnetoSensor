// Copyright 2022-2024 Rik Essenius
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
// except in compliance with the License. You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and limitations under the License.

// Gets a sample every 10 milliseconds and sends it over the serial port. Can be viewed in the serial plotter.
// At 115200 baud, printing shouldn't take more than about 120 nanoseconds, so that should not interfere.
//
// Connect the sensor VCC to the power pin, and GND, SCL and SDA to the corresponding ESP32 board pin.
// The power pin was used to make it possible to reset the sensor by power cycling it.
// If you want to work directly off 3.3V that works too - then you can eliminate the digitalWrite to the power pin.

#include <Wire.h>
#ifdef ESP32
#include <MagnetoSensorQmc.h>
#else
#include "../src/MagnetoSensorQmc.h"
#endif

using namespace MagnetoSensors;

namespace Qmc5883LDemo {

    constexpr uint8_t PowerPin = 15;
    constexpr int SampleTime = 10000;

    MagnetoSensorQmc sensor(&Wire);

    void printSampleWithDuration(const SensorData& sample, const unsigned long sampleDuration) {
        Serial.printf("x:%d, y:%d, z:%d", sample.x, sample.y, sample.z);
        Serial.printf(", duration:%d\n", sampleDuration);
    }

    void readSample(const bool print = true) {
        const unsigned long startTime = micros();
        SensorData sample;
        sensor.read(sample);
        const auto sampleDuration = micros() - startTime;
        if (print) {
            printSampleWithDuration(sample, sampleDuration);
        }
        while (micros() - startTime < SampleTime);
    }

    void setup() {
        Serial.begin(115200);
        pinMode(PowerPin, OUTPUT);
        digitalWrite(PowerPin, HIGH);
        delay(5); // allow sensor to start up
        Wire.begin();

        // These configure commands are not needed as they use defaults; just for showing how it works.
        // Configuration needs to be done before begin.
        sensor.configureRange(QmcRange8G);
        sensor.configureRate(QmcRate100Hz);
        sensor.configureOverSampling(QmcSampling512);
        sensor.begin();

        // Get the sensor started. The first few results might not be reliable
        for (int i = 0; i < 5; i++) {
            readSample(false);
        }
        Serial.println("Init complete");
    }

    void loop() {
        readSample();
    }
}
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

// Disabling a few static analysis findings for code clarity. The issues are related to:
// * conversions that might in theory go wrong, but the sensor won't return too high values.
// * using enum values to do bitwise manipulations

// ReSharper disable CppClangTidyBugproneNarrowingConversions
// ReSharper disable CppClangTidyClangDiagnosticImplicitIntConversion
// ReSharper disable CppClangTidyClangDiagnosticEnumEnumConversion
// ReSharper disable CppClangTidyBugproneSuspiciousEnumUsage

#include "MagnetoSensorHmc.h"
#include "Wire.h"

namespace MagnetoSensors {
    MagnetoSensorHmc::MagnetoSensorHmc(TwoWire* wire) : MagnetoSensor(DefaultAddress, wire) {}

    void MagnetoSensorHmc::configure(const HmcRange range, const HmcBias bias) const {
        setRegister(HmcControlA, _overSampling | _rate | bias);
        setRegister(HmcControlB, range);
    }

    void MagnetoSensorHmc::configureRange(const HmcRange range) {
        _range = range;
    }

    void MagnetoSensorHmc::configureOverSampling(const HmcOverSampling overSampling) {
        _overSampling = overSampling;
    }

    void MagnetoSensorHmc::configureRate(const HmcRate rate) {
        _rate = rate;
    }

    double MagnetoSensorHmc::getGain() const {
        return getGain(_range);
    }

    HmcRange MagnetoSensorHmc::getRange() const {
        return _range;
    }

    int MagnetoSensorHmc::getNoiseRange() const {
        switch (_range) {
            case HmcRange0_88: return 8;
            case HmcRange1_3:
            case HmcRange1_9: return 5;
            case HmcRange2_5:
            case HmcRange4_0: return 4;
            case HmcRange4_7: return 3; // was 4
            case HmcRange5_6:
            case HmcRange8_1: return 2;
        }
        // should not happen
        return 0;
    }

    double MagnetoSensorHmc::getGain(const HmcRange range) {
        switch (range) {
            case HmcRange0_88: return 1370.0;
            case HmcRange1_3: return 1090.0;
            case HmcRange1_9: return 820.0;
            case HmcRange2_5: return 660.0;
            case HmcRange4_0: return 440.0;
            case HmcRange4_7: return 390.0;
            case HmcRange5_6: return 330.0;
            case HmcRange8_1: return 230.0;
        }
        // should not happen
        return 0;
    }

    void MagnetoSensorHmc::getTestMeasurement(SensorData& reading) {
        startMeasurement();
        delay(5);
        read(reading);
    }

    short MagnetoSensorHmc::readWord() const {
        constexpr byte BitsPerByte = 8;

        // preventing compiler optimization as read() has side effects
        // order is MSB, LSB
        short result = _wire->read() << BitsPerByte;
        result |= _wire->read();

        // harmonize saturation values across sensors
        if (result <= Saturated) {
            result = SHRT_MIN;
        }
        return result;
    }

    bool MagnetoSensorHmc::read(SensorData& sample) {
        startMeasurement();

        _wire->beginTransmission(_address);
        _wire->write(HmcData);
        _wire->endTransmission();

        //Read data from each axis, 2 registers per axis
        // order: x MSB, x LSB, z MSB, z LSB, y MSB, y LSB
        constexpr int BytesToRead = 6;
        _wire->requestFrom(_address, BytesToRead, StopAfterSend);
        const auto timestamp = micros();
        while (_wire->available() < BytesToRead) {
            if (micros() - timestamp > 10) return false;
        }
        sample.x = readWord();
        sample.z = readWord();
        sample.y = readWord();
        return true;
    }

    void MagnetoSensorHmc::softReset() {
        configure(_range, HmcNone);
        SensorData sample{};
        getTestMeasurement(sample);
    }

    void MagnetoSensorHmc::startMeasurement() const {
        setRegister(HmcMode, HmcSingle);
    }

    bool MagnetoSensorHmc::testInRange(const SensorData& sample) {
        constexpr short LowThreshold = 243;
        constexpr short HighThreshold = 575;

        return
            sample.x >= LowThreshold && sample.x <= HighThreshold &&
            sample.y >= LowThreshold && sample.y <= HighThreshold &&
            sample.z >= LowThreshold && sample.z <= HighThreshold;
    }

    bool MagnetoSensorHmc::test() {
        SensorData sample{};

        configure(HmcRange4_7, HmcPositive);

        // read old value (still with old settings) 
        getTestMeasurement(sample);
        // the first one with new settings may still be a bit off
        getTestMeasurement(sample);

        // now do the test
        getTestMeasurement(sample);
        const bool passed = testInRange(sample);

        // end self test mode
        configure(_range, HmcNone);
        // skip the final measurement with the old gain
        getTestMeasurement(sample);

        return passed;
    }

    bool MagnetoSensorHmc::handlePowerOn() {
        return test();
    }

    bool MagnetoSensorHmc::increaseRange() {
        if (_range == HmcRange8_1) return false;
        _range = static_cast<HmcRange>(static_cast<int>(_range) + 32);
        softReset();
        return true;
    }
}

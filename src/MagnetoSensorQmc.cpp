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

#include "MagnetoSensorQmc.h"
#include "Wire.h"

namespace MagnetoSensors {
    constexpr int SoftReset = 0x80;

    MagnetoSensorQmc::MagnetoSensorQmc(TwoWire* wire) : MagnetoSensor(DefaultAddress, wire) {}

    bool MagnetoSensorQmc::configure() const {
        setRegister(QmcSetReset, 0x01);
        setRegister(QmcControl1, QmcContinuous | _rate | _range | _overSampling);
        return true;
    }

    void MagnetoSensorQmc::configureOverSampling(const QmcOverSampling overSampling) {
        _overSampling = overSampling;
    }

    void MagnetoSensorQmc::configureRange(const QmcRange range) {
        _range = range;
    }

    void MagnetoSensorQmc::configureRate(const QmcRate rate) {
        _rate = rate;
    }

    // if we ever need DataReady, use (getRegister(QmcStatus) & 0x01) != 0;

    double MagnetoSensorQmc::getGain() const {
        return getGain(_range);
    }

    double MagnetoSensorQmc::getGain(const QmcRange range) {
        if (range == QmcRange8G) return 3000.0;
        return 12000.0;
    }

    QmcRange MagnetoSensorQmc::getRange() const {
        return _range;
    }

    short MagnetoSensorQmc::readWord() const {
        constexpr byte BitsPerByte = 8;

        // preventing compiler optimization as read() has side effects
        // order is LSB, MSB
        short result = _wire->read();
        result |= _wire->read() << BitsPerByte;
        // if we got a positive saturation, shift it to SHRT_MIN as SHRT_MAX means an error
        if (result == SHRT_MAX) result = SHRT_MIN;
        return result;
    }

    bool MagnetoSensorQmc::read(SensorData& sample) {
        _wire->beginTransmission(_address);
        _wire->write(QmcData);
        _wire->endTransmission();

        constexpr size_t BytesToRead = 6;
        // Read data from each axis, 2 registers per axis
        // order: x LSB, x MSB, y LSB, y MSB, z LSB, z MSB
        _wire->requestFrom(_address, BytesToRead, StopAfterSend);
        while (_wire->available() < BytesToRead) {}
        sample.x = readWord();
        sample.y = readWord();
        sample.z = readWord();
        return true;
    }

    void MagnetoSensorQmc::softReset() {
        setRegister(QmcControl2, SoftReset);
        static_cast<void>(configure());
    }

    int MagnetoSensorQmc::getNoiseRange() const {
        // only checked on 8 Gauss
        return 60;
    }
}

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
#ifndef MAGNETOSENSOR_MOCK_H
#define MAGNETOSENSOR_MOCK_H

#include "../src/MagnetoSensor.h"

namespace MagnetoSensorsTest {
    using namespace MagnetoSensors;

    class MagnetoSensorMock final : public MagnetoSensor {
    public:
        MagnetoSensorMock() : MagnetoSensor(0, nullptr) {}

        bool isOn() override;
        double getGain() const override { return 1.0; }
        int getNoiseRange() const override { return 1; }
        bool read(SensorData& sample) override { return false; }
        void softReset() override {}

    private:
        int _calls = 0;
    };
}

#endif // MAGNETOSENSOR_MOCK_H

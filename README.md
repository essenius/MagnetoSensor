# MagnetoSensor

This repo provides a driver for HMC5883L and QMC5883L magneto-sensors

The two main classes are `MagnetoSensorHmc` and `MagnetoSensorQmc`. There is also a `MagnetoSensorNull` that can be used e.g. when no sensor can be detected.
It uses I2C, so therefore the Arduino Wire class is also in use.

#ifndef AM1001_H
#define AM1001_H

#include <Arduino.h>

class AM1001 {
public:
    /**
     * @brief Construct a new AM1001 object
     * 
     * @param humidityPin Analog pin for humidity (Yellow wire)
     * @param tempPin Analog pin for NTC thermistor (White wire)
     */
    AM1001(uint8_t humidityPin, uint8_t tempPin);

    /**
     * @brief Initialize the sensor (sets pin modes)
     */
    void begin();

    /**
     * @brief Read humidity in percentage
     * 
     * @return float Humidity percentage (0-100%)
     */
    float readHumidity();

    /**
     * @brief Read temperature in Celsius
     * 
     * @return float Temperature in Celsius
     */
    float readTemperature();

private:
    uint8_t _hPin;
    uint8_t _tPin;

    // Constants for temperature calculation (10K NTC)
    const float SERIES_RESISTOR = 10000.0f; // 10K Ohm
    const float THERMISTOR_NOMINAL = 10000.0f; 
    const float TEMPERATURE_NOMINAL = 25.0f;
    const float B_COEFFICIENT = 3950.0f;
    const uint16_t ADC_MAX = 4095; // 12-bit ADC
};

#endif // AM1001_H

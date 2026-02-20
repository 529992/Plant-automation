#include "AM1001.h"
#include <math.h>

AM1001::AM1001(uint8_t humidityPin, uint8_t tempPin) : _hPin(humidityPin), _tPin(tempPin) {}

void AM1001::begin() {
    pinMode(_hPin, INPUT);
    pinMode(_tPin, INPUT);
}

float AM1001::readHumidity() {
    int raw = analogRead(_hPin);
    // ESP32 ADC is 12-bit (0-4095). 
    // AM1001 humidity output is approx 0.6V to 2.7V for 20-90% RH.
    // 0.03V per 1% RH is a common specification for these sensors.
    
    float voltage = (raw * 3.3f) / (float)ADC_MAX;
    
    // Simple linear mapping: 0V -> 0%, 3V -> 100% (Check datasheet specifics for AM1001)
    // Most AM1001 modules map 0-3V to 0-100% RH.
    float humidity = (voltage / 3.0f) * 100.0f;
    
    if (humidity < 0) humidity = 0;
    if (humidity > 100) humidity = 100;
    
    return humidity;
}

float AM1001::readTemperature() {
    int raw = analogRead(_tPin);
    if (raw == 0) return -273.15f; // Avoid division by zero

    // Assuming NTC is in a voltage divider with SERIES_RESISTOR (10K) to 3.3V
    // V_out = V_cc * R_ntc / (R_series + R_ntc)
    // R_ntc = R_series / (ADC_MAX / raw - 1)
    
    float resistance = SERIES_RESISTOR / ((float)ADC_MAX / raw - 1);
    
    // Steinhart-Hart equation (simplified B-parameter)
    float steinhart;
    steinhart = resistance / THERMISTOR_NOMINAL;     // (R/Ro)
    steinhart = log(steinhart);                      // ln(R/Ro)
    steinhart /= B_COEFFICIENT;                      // 1/B * ln(R/Ro)
    steinhart += 1.0f / (TEMPERATURE_NOMINAL + 273.15f); // + (1/To)
    steinhart = 1.0f / steinhart;                    // Invert
    steinhart -= 273.15f;                            // convert to C

    return steinhart;
}

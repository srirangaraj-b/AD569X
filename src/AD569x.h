#ifndef AD569X_H
#define AD569X_H

#include <Arduino.h>
#include <Wire.h>

// Supported device models
enum class AD569xModel : uint8_t {
    AD5691 = 0, // 1-channel, 16-bit
    AD5692,     // 1-channel, 14-bit
    AD5693,     // 1-channel, 16-bit
    AD5694,     // 4-channel, 12-bit
    AD5695,     // 4-channel, 14-bit
    AD5696      // 4-channel, 16-bit
};

// Output Gain settings
enum class AD569xGain : uint8_t {
    GAIN_1X = 0, // VOUT = VREF * Code / 2^N
    GAIN_2X = 1  // VOUT = 2 * VREF * Code / 2^N
};

// Power-down modes
enum class AD569xPowerDownMode : uint8_t {
    NORMAL       = 0,
    PD_1K_GND    = 1, // 1 kOhm to GND
    PD_100K_GND  = 2, // 100 kOhm to GND
    PD_THREE_STATE = 3 // High-impedance / Three-state
};

class AD569x {
public:
    AD569x(AD569xModel model = AD569xModel::AD5694);
    ~AD569x() = default;

    // Initialization
    bool begin(uint8_t address = 0x0C);
    bool begin(TwoWire &wire, uint8_t address = 0x0C);

    // Diagnostics
    bool isConnected();
    AD569xModel getModel() const;
    uint8_t getResolution() const;
    uint8_t getChannelCount() const;
    uint8_t getI2CAddress() const;

    // Raw DAC Operations
    bool setCode(uint8_t channel, uint16_t code);
    bool writeCode(uint8_t channel, uint16_t code);
    bool update(uint8_t channel);
    bool updateAll();

    // Voltage Operations & Scaling
    bool setVoltage(uint8_t channel, float voltage);
    uint16_t voltageToCode(float voltage) const;
    float codeToVoltage(uint16_t code) const;

    // Configuration / Hardware Limits
    void setReferenceVoltage(float vref);
    void setCustomSpan(float minVoltage, float maxVoltage);
    bool setGain(AD569xGain gain);

    // Power Management
    bool powerDown(uint8_t channel, AD569xPowerDownMode mode);
    bool reset();

private:
    TwoWire *_wire;
    uint8_t _i2cAddress;
    AD569xModel _model;
    
    uint8_t _resolution;
    uint8_t _channelCount;
    uint16_t _maxCode;

    float _vref;
    float _customMinVoltage;
    float _customMaxVoltage;
    bool _useCustomSpan;
    AD569xGain _gain;

    // Internal communication helper
    bool writeCommand(uint8_t cmdByte, uint16_t dataWord);
    bool validateChannel(uint8_t channel) const;
};

#endif // AD569X_H
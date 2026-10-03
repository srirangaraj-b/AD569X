#include "AD569x.h"

#define AD569X_CMD_WRITE_INPUT      0x10
#define AD569X_CMD_UPDATE_DAC       0x20
#define AD569X_CMD_WRITE_UPDATE     0x30
#define AD569X_CMD_POWER_DOWN       0x40
#define AD569X_CMD_RESET            0x50
#define AD569X_CMD_GAIN             0x60

#define AD569X_CHAN_0               0x01
#define AD569X_CHAN_1               0x02
#define AD569X_CHAN_2               0x04
#define AD569X_CHAN_3               0x08
#define AD569X_CHAN_ALL             0x0F

AD569x::AD569x(AD569xModel model) 
    : _wire(&Wire), 
      _i2cAddress(0x0C), 
      _model(model), 
      _vref(2.5f), 
      _customMinVoltage(0.0f), 
      _customMaxVoltage(5.0f), 
      _useCustomSpan(false), 
      _gain(AD569xGain::GAIN_1X) 
{
    switch (_model) {
        case AD569xModel::AD5691:
        case AD569xModel::AD5693:
            _resolution = 16;
            _channelCount = 1;
            break;
        case AD569xModel::AD5692:
            _resolution = 14;
            _channelCount = 1;
            break;
        case AD569xModel::AD5694:
            _resolution = 12;
            _channelCount = 4;
            break;
        case AD569xModel::AD5695:
            _resolution = 14;
            _channelCount = 4;
            break;
        case AD569xModel::AD5696:
            _resolution = 16;
            _channelCount = 4;
            break;
    }
    _maxCode = (1UL << _resolution) - 1;
}

bool AD569x::begin(uint8_t address) {
    return begin(Wire, address);
}

bool AD569x::begin(TwoWire &wire, uint8_t address) {
    _wire = &wire;
    _i2cAddress = address;
    _wire->begin();
    return isConnected();
}

bool AD569x::isConnected() {
    _wire->beginTransmission(_i2cAddress);
    return (_wire->endTransmission() == 0);
}

AD569xModel AD569x::getModel() const {
    return _model;
}

uint8_t AD569x::getResolution() const {
    return _resolution;
}

uint8_t AD569x::getChannelCount() const {
    return _channelCount;
}

uint8_t AD569x::getI2CAddress() const {
    return _i2cAddress;
}

bool AD569x::validateChannel(uint8_t channel) const {
    return (channel < _channelCount);
}

bool AD569x::writeCommand(uint8_t cmdByte, uint16_t dataWord) {
    _wire->beginTransmission(_i2cAddress);
    _wire->write(cmdByte);
    // Upper data byte
    _wire->write((uint8_t)((dataWord >> 8) & 0xFF));
    // Lower data byte
    _wire->write((uint8_t)(dataWord & 0xFF));
    return (_wire->endTransmission() == 0);
}

bool AD569x::setCode(uint8_t channel, uint16_t code) {
    if (!validateChannel(channel)) return false;
    if (code > _maxCode) code = _maxCode;

    // Left-align code to 16-bit boundary as required by AD569x shift register
    uint16_t shiftedCode = code << (16 - _resolution);
    uint8_t chanBit = (1 << channel);

    return writeCommand(AD569X_CMD_WRITE_UPDATE | chanBit, shiftedCode);
}

bool AD569x::writeCode(uint8_t channel, uint16_t code) {
    if (!validateChannel(channel)) return false;
    if (code > _maxCode) code = _maxCode;

    uint16_t shiftedCode = code << (16 - _resolution);
    uint8_t chanBit = (1 << channel);

    return writeCommand(AD569X_CMD_WRITE_INPUT | chanBit, shiftedCode);
}

bool AD569x::update(uint8_t channel) {
    if (!validateChannel(channel)) return false;
    uint8_t chanBit = (1 << channel);
    return writeCommand(AD569X_CMD_UPDATE_DAC | chanBit, 0x0000);
}

bool AD569x::updateAll() {
    return writeCommand(AD569X_CMD_UPDATE_DAC | AD569X_CHAN_ALL, 0x0000);
}

void AD569x::setReferenceVoltage(float vref) {
    _vref = vref;
}

void AD569x::setCustomSpan(float minVoltage, float maxVoltage) {
    _customMinVoltage = minVoltage;
    _customMaxVoltage = maxVoltage;
    _useCustomSpan = true;
}

bool AD569x::setGain(AD569xGain gain) {
    _gain = gain;
    uint16_t data = (_gain == AD569xGain::GAIN_2X) ? 0x0001 : 0x0000;
    return writeCommand(AD569X_CMD_GAIN, data);
}

uint16_t AD569x::voltageToCode(float voltage) const {
    if (_useCustomSpan) {
        if (voltage <= _customMinVoltage) return 0;
        if (voltage >= _customMaxVoltage) return _maxCode;
        float span = _customMaxVoltage - _customMinVoltage;
        return (uint16_t)(((voltage - _customMinVoltage) / span) * (float)_maxCode);
    } else {
        float effectiveVref = _vref * ((_gain == AD569xGain::GAIN_2X) ? 2.0f : 1.0f);
        if (voltage <= 0.0f) return 0;
        if (voltage >= effectiveVref) return _maxCode;
        return (uint16_t)((voltage / effectiveVref) * (float)_maxCode);
    }
}

float AD569x::codeToVoltage(uint16_t code) const {
    if (code > _maxCode) code = _maxCode;
    if (_useCustomSpan) {
        float span = _customMaxVoltage - _customMinVoltage;
        return _customMinVoltage + ((float)code / (float)_maxCode) * span;
    } else {
        float effectiveVref = _vref * ((_gain == AD569xGain::GAIN_2X) ? 2.0f : 1.0f);
        return ((float)code / (float)_maxCode) * effectiveVref;
    }
}

bool AD569x::setVoltage(uint8_t channel, float voltage) {
    uint16_t code = voltageToCode(voltage);
    return setCode(channel, code);
}

bool AD569x::powerDown(uint8_t channel, AD569xPowerDownMode mode) {
    if (!validateChannel(channel) && channel != 0xFF) return false;
    uint8_t chanBit = (channel == 0xFF) ? AD569X_CHAN_ALL : (1 << channel);
    uint16_t modeVal = (uint16_t)mode << 4;
    return writeCommand(AD569X_CMD_POWER_DOWN | chanBit, modeVal);
}

bool AD569x::reset() {
    return writeCommand(AD569X_CMD_RESET, 0x0000);
}
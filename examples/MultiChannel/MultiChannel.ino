#include <AD569x.h>

AD569x dac(AD569xModel::AD5694); // 4-channel DAC

void setup() {
    Serial.begin(115200);
    while (!Serial);
    dac.begin(0x0C);
    dac.setCustomSpan(0.0f, 5.0f);

    // Set each channel to a different voltage
    dac.setVoltage(0, 1.0f);
    dac.setVoltage(1, 2.0f);
    dac.setVoltage(2, 3.0f);
    dac.setVoltage(3, 4.0f);
    
    Serial.println("All 4 channels configured.");
}

void loop() {
}
#include <AD569x.h>

AD569x dac(AD569xModel::AD5694);

void setup() {
    Serial.begin(115200);
    while (!Serial);
    dac.begin(0x0C);

    // Power down channel 0 with a 1k resistor to GND
    dac.powerDown(0, AD569xPowerDownMode::PD_1K_GND);
    Serial.println("Channel 0 powered down.");
}

void loop() {
}
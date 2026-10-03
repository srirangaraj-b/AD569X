#include <AD569x.h>

AD569x dac(AD569xModel::AD5694);

void setup() {
    Serial.begin(115200);
    while (!Serial);

    dac.begin(0x0C);

    Serial.println("=== AD569x Device Info ===");
    Serial.print("Resolution: ");
    Serial.print(dac.getResolution());
    Serial.println(" bits");

    Serial.print("Channels: ");
    Serial.println(dac.getChannelCount());

    Serial.print("I2C Address: 0x");
    Serial.println(dac.getI2CAddress(), HEX);
}

void loop() {
}
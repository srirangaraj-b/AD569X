#include <AD569x.h>

AD569x dac(AD569xModel::AD5694);
const float voltages[] = {0.0f, 1.3f, 2.5f, 3.75f, 5.0f};
const int numVoltages = 5;

void setup() {
    Serial.begin(115200);
    while (!Serial);

    dac.begin(0x0C);

    dac.setCustomSpan(0.0f, 5.0f);
}

void loop() {
    for (int i = 0; i < numVoltages; i++) {
        dac.setVoltage(0, voltages[i]);
        Serial.print("Channel 0 set to: ");
        Serial.print(voltages[i]);
        Serial.println(" V");
        delay(2000);
    }
}
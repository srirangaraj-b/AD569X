#include <AD569x.h>

AD569x dac(AD569xModel::AD5694);

void setup() {
    Serial.begin(115200);
    while (!Serial);

    if (!dac.begin(0x0C)) {
        Serial.println("AD569x connection failed!");
        while (1);
    }
    Serial.println("AD569x connected successfully.");

    // Write raw mid-scale code (12-bit max = 0xFFF, mid = 0x800)
    dac.setCode(0, 0x800);
}

void loop() {
}
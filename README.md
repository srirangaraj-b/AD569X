# AD569x Arduino Library

A professional, lightweight, and extensible Arduino library for the Analog Devices **AD569x** nanoDAC+ I2C DAC family. Designed from the ground up for performance, type safety, and seamless support across 1-channel and 4-channel, 12-bit, 14-bit, and 16-bit variants.

---

## Table of Contents

1. [Library Description](https://www.google.com/search?q=%23library-description)
2. [Supported Devices](https://www.google.com/search?q=%23supported-devices)
3. [Hardware Requirements & Wiring](https://www.google.com/search?q=%23hardware-requirements--wiring)
4. [Installation](https://www.google.com/search?q=%23installation)
5. [Quick Start](https://www.google.com/search?q=%23quick-start)
6. [API Reference](https://www.google.com/search?q=%23api-reference)
7. [Device Capability Table](https://www.google.com/search?q=%23device-capability-table)
8. [Voltage Calculation & Scaling Explanation](https://www.google.com/search?q=%23voltage-calculation--scaling-explanation)
9. [I2C Address Explanation](https://www.google.com/search?q=%23i2c-address-explanation)
10. [Reference & Gain Configuration](https://www.google.com/search?q=%23reference--gain-configuration)
11. [Power-Down Modes](https://www.google.com/search?q=%23power-down-modes)
12. [Multi-Channel Usage](https://www.google.com/search?q=%23multi-channel-usage)
13. [Error Handling & Troubleshooting](https://www.google.com/search?q=%23error-handling--troubleshooting)
14. [Example Projects](https://www.google.com/search?q=%23example-projects)

---

## 1. Library Description

The **AD569x** library provides a clean, robust, and object-oriented interface to control Analog Devices' high-precision I2C nanoDAC+ converters. Key architectural benefits include:

* **Descriptor-Driven Architecture:** A single unified driver handles 12, 14, and 16-bit architectures without code duplication.
* **Zero Dynamic Memory:** No heap allocation (`malloc`/`new`) or `String` class overhead, ensuring zero memory fragmentation and deterministic timing.
* **Advanced Voltage Spans:** Supports standard internal/external reference math as well as custom calibration spans (e.g., handling hardware op-amp level shifters that scale 0–3.3V DAC outputs to 0–5V).

---

## 2. Supported Devices

The library supports the entire standard AD569x family:

| Model | Channels | Resolution | Internal Reference Option |
| --- | --- | --- | --- |
| **AD5691 / AD5691R** | 1 | 16-bit | Yes ('R' variant) |
| **AD5692 / AD5692R** | 1 | 14-bit | Yes ('R' variant) |
| **AD5693 / AD5693R** | 1 | 16-bit | Yes ('R' variant) |
| **AD5694 / AD5694R** | 4 | 12-bit | Yes ('R' variant) |
| **AD5695 / AD5695R** | 4 | 14-bit | Yes ('R' variant) |
| **AD5696 / AD5696R** | 4 | 16-bit | Yes ('R' variant) |

---

## 3. Hardware Requirements & Wiring

### Typical I2C Pinout

* **SDA:** Connect to micro-controller SDA line (with external pull-up resistors, typically 4.7kΩ).
* **SCL:** Connect to micro-controller SCL line (with external pull-up resistors).
* **VCC / VDD:** 2.7V to 5.5V supply voltage.
* **GND:** Common system ground.

> [!WARNING]
> Always check your specific breakout board's datasheet regarding logic level translation if you are running a 5V Arduino (like the Uno) with a 3.3V DAC variant.

---

## 4. Installation

### Method 1: Arduino Library Manager (Recommended)

1. Open the **Arduino IDE**.
2. Go to **Tools > Manage Libraries...**
3. Search for `AD569x`.
4. Click **Install**.

### Method 2: Manual Installation

1. Download or clone this repository.
2. Place the unzipped `AD569x` folder into your `Arduino/libraries/` directory.
3. Restart the Arduino IDE.

---

## 5. Quick Start

```cpp
#include <AD569x.h>

// Instantiate for the 4-channel 12-bit AD5694
AD569x dac(AD569xModel::AD5694);

void setup() {
    Serial.begin(115200);
    
    // Initialize Wire and verify I2C connection at address 0x0C
    if (!dac.begin(0x0C)) {
        Serial.println("AD569x not found!");
        while (1);
    }

    // Set channel 0 to 2.5V (using default span)
    dac.setVoltage(0, 2.5f);
}

void loop() {}

```

---

## 6. API Reference

### Initialization & Diagnostics

* `bool begin(uint8_t address = 0x0C);` — Initializes the default `Wire` interface and checks device responsiveness.
* `bool begin(TwoWire &wire, uint8_t address = 0x0C);` — Initializes with a custom I2C bus (`Wire1`, `Wire2`, etc.).
* `bool isConnected();` — Pings the DAC via I2C to confirm active communication.
* `AD569xModel getModel() const;` — Returns the configured model enum.
* `uint8_t getResolution() const;` — Returns bit resolution (12, 14, or 16).
* `uint8_t getChannelCount() const;` — Returns total channels (1 or 4).
* `uint8_t getI2CAddress() const;` — Returns active I2C address.

### DAC Output Control

* `bool setCode(uint8_t channel, uint16_t code);` — Writes and immediately updates a DAC channel with a raw code.
* `bool writeCode(uint8_t channel, uint16_t code);` — Writes to the input register without updating output yet.
* `bool update(uint8_t channel);` — Triggers output update from the input register for a specific channel.
* `bool updateAll();` — Simultaneously updates all channels.

### Voltage & Scaling

* `bool setVoltage(uint8_t channel, float voltage);` — Converts voltage to code and sets channel output.
* `uint16_t voltageToCode(float voltage) const;` — Translates a target voltage into a raw DAC register code.
* `float codeToVoltage(uint16_t code) const;` — Translates a raw code back to an expected voltage output.
* `void setReferenceVoltage(float vref);` — Sets reference voltage baseline (default `2.5f`).
* `void setCustomSpan(float minVoltage, float maxVoltage);` — Overrides reference calculation with absolute hardware span limits (e.g., 0V to 5V).

### Configuration & Power Management

* `bool setGain(AD569xGain gain);` — Sets gain option (`GAIN_1X` or `GAIN_2X`).
* `bool powerDown(uint8_t channel, AD569xPowerDownMode mode);` — Places channel(s) into specialized power-down states.
* `bool reset();` — Performs a software reset of the device registers.

---

## 7. Device Capability Table

| Feature / Model | AD5691/3 | AD5692 | AD5694 | AD5695 | AD5696 |
| --- | --- | --- | --- | --- | --- |
| **Resolution** | 16-bit | 14-bit | 12-bit | 14-bit | 16-bit |
| **Max Code ($2^N - 1$)** | 65535 | 16383 | 4095 | 16383 | 65535 |
| **Channels** | 1 | 1 | 4 | 4 | 4 |
| **Default I2C Address** | 0x0C | 0x0C | 0x0C | 0x0C | 0x0C |

---

## 8. Voltage Calculation & Scaling Explanation

By default, the library calculates voltages using the formula:


$$V_{out} = \frac{\text{Code}}{2^N - 1} \times V_{ref} \times \text{Gain}$$

However, if your hardware design utilizes an external operational amplifier circuit that scales or shifts the DAC output range (for instance, mapping a 0–3.3V native output up to 0–5.0V), standard reference math will yield incorrect results.

To fix this cleanly without rewriting internal formulas, use `setCustomSpan`:

```cpp
// Tells the library your hardware outputs 0V at code 0 and 5V at max code
dac.setCustomSpan(0.0f, 5.0f);

```

---

## 9. I2C Address Explanation

The AD569x family defaults to I2C address **`0x0C`** (decimal 12). Depending on how hardware address pins (`A0`, `A1`) are tied on your breakout module, the address can typically be reconfigured between `0x0C` and surrounding addresses. Pass your custom address directly into `begin()`:

```cpp
dac.begin(0x0D); // If hardware address pin configuration changes base address

```

---

## 10. Reference & Gain Configuration

* **Gain Settings:**
* `AD569xGain::GAIN_1X`: Output spans from $0\text{V}$ to $V_{ref}$.
* `AD569xGain::GAIN_2X`: Output spans from $0\text{V}$ to $2 \times V_{ref}$.


* **Setting Gain via Code:**
```cpp
dac.setGain(AD569xGain::GAIN_2X);

```



---

## 11. Power-Down Modes

The library supports low-power management states for power-sensitive applications:

* `AD569xPowerDownMode::NORMAL` — Active operation.
* `AD569xPowerDownMode::PD_1K_GND` — Powered down with a $1\,\text{k}\Omega$ resistor to GND.
* `AD569xPowerDownMode::PD_100K_GND` — Powered down with a $100\,\text{k}\Omega$ resistor to GND.
* `AD569xPowerDownMode::PD_THREE_STATE` — High-impedance output state.

Usage example:

```cpp
// Power down channel 1 with 1k pull-down to GND
dac.powerDown(1, AD569xPowerDownMode::PD_1K_GND);

```

---

## 12. Multi-Channel Usage

For multi-channel models (AD5694, AD5695, AD5696), you can target individual channels independently:

```cpp
dac.setVoltage(0, 1.0f); // Channel 0 -> 1.0V
dac.setVoltage(1, 2.0f); // Channel 1 -> 2.0V
dac.setVoltage(2, 3.0f); // Channel 2 -> 3.0V
dac.setVoltage(3, 4.0f); // Channel 3 -> 4.0V

```

---

## 13. Error Handling & Troubleshooting

* **I2C Communication Failure (`isConnected()` returns `false`):**
* Verify wiring connections for SDA and SCL.
* Check that pull-up resistors are present on the I2C bus.
* Confirm the correct I2C address using an I2C scanner sketch.


* **Incorrect Output Voltages:**
* Ensure your reference voltage configuration matches your hardware design (`setReferenceVoltage`).
* If using level-shifting amplifiers, verify you configured `setCustomSpan()`.



---

## 14. Example Projects

Included examples in the library package:

1. **`BasicWrite`**: Demonstrates raw register code writing.
2. **`SetVoltage`**: Steps through multiple calibrated engineering voltages.
3. **`MultiChannel`**: Configures individual outputs across multiple channels simultaneously.
4. **`DeviceInfo`**: Queries chip resolution, channel configuration, and address parameters over Serial.
5. **`PowerDown`**: Demonstrates low-power mode transitions.

## License

This project is open-source and available under the MIT License.
# ESP32-S3-Zero Mini Projects

Small, self-contained Arduino projects for the **Waveshare ESP32-S3-Zero** mini development board.

Based on the Waveshare tutorial: [Section 4 — ADC Analog Input](https://docs.waveshare.com/ESP32-Arduino-Tutorials/Analog-Input)

---

## Projects

| Project | Description |
| --- | --- |
| [`Analog_Input`](Analog_Input/Analog_Input.ino) | Turn a potentiometer on GPIO7 → read the ADC value (0–4095) and the voltage (mV) over Serial |

---

## Analog_Input

A potentiometer acts as a variable voltage divider: rotating the knob outputs a continuously changing voltage between **0V and 3.3V** on the middle pin. The ESP32's **ADC** (Analog-to-Digital Converter) turns that analog voltage into a number the program can use.

### Wiring

| Component | ESP32-S3-Zero pin |
| --- | --- |
| Potentiometer VCC (one outer leg) | **3.3V** |
| Potentiometer GND (other outer leg) | **GND** |
| Potentiometer signal (middle leg) | **GPIO7** |

**Components required:** 1× potentiometer, 1× breadboard, jumper wires, ESP32-S3-Zero board.

- **Fully counter-clockwise** → output ≈ 0V → ADC reads ≈ **0**
- **Fully clockwise** → output ≈ 3.3V → ADC reads ≈ **4095**
- The middle pin of a potentiometer is a *voltage divider* wiper: it picks off a fraction of the 3.3V applied across the two outer legs, and that fraction moves smoothly as the knob turns.

### ADC pins on the ESP32-S3

Not every GPIO can read analog. On the **ESP32-S3**, ADC1 covers **GPIO1 – GPIO10** (recommended, doesn't clash with Wi-Fi) and ADC2 covers GPIO11 – GPIO20 (unavailable while Wi-Fi is active). GPIO7 used here is an ADC1 pin, so no `pinMode()` setup is required.

| Chip | Recommended (ADC1) | Secondary (ADC2) |
| --- | --- | --- |
| **ESP32-S3** | GPIO1 – GPIO10 | GPIO11 – GPIO20 |
| ESP32 | GPIO32 – GPIO39 | GPIO0, 2, 4, 12–15, 25–27 |
| ESP32-C3 | GPIO0 – GPIO4 | GPIO5 (not available) |
| ESP32-C6 | GPIO0 – GPIO6 | — |

### Code

```cpp
const int potentiometerPin = 7;  // Define the pins connected to the potentiometer

void setup() {
  Serial.begin(9600);                // Initialize serial communication and set the baud rate to 9600
}

void loop() {
  int analogValue = analogRead(potentiometerPin);            // Read the analog value of potentiometer (0-4095)
  int analogVolts = analogReadMilliVolts(potentiometerPin);  // Read the voltage value on the pin, in millivolts (mV)

  // Print the first label and value
  Serial.print("ADC_Value:");
  Serial.print(analogValue);

  // Print the separator
  Serial.print(",");

  // Print the second tag and value, and end with println()
  Serial.print("Voltage_mV:");
  Serial.println(analogVolts);

  delay(100);  // Delay 0.1 seconds to avoid serial port scrolling too fast
}
```

### How it works

**Analog vs. digital:** a digital signal only has two states (HIGH/LOW), like a light switch. An analog signal — potentiometer position, temperature, light level, sound — changes *continuously*. `digitalRead()` cannot capture that, so an ADC is needed.

**Resolution:** the ESP32 ADC is **12-bit**, so it divides 0–3.3V into **2¹² = 4096** levels, giving readings of **0 … 4095**. Think of it as a ruler with 4096 tick marks: 0V → 0, 3.3V → 4095, and everything in between is proportional.

**Key functions:**

1. `analogRead(potentiometerPin)`
   - Reads the analog voltage on the pin and returns an integer **0–4095**.
   - **No `pinMode()` needed** — ADC pins are configured automatically before the first read.

2. `analogReadMilliVolts(potentiometerPin)`
   - An ESP32-specific function that reads the same pin but returns the value already converted to **millivolts (mV)**.
   - It applies the chip's **factory calibration data**, so it is noticeably more accurate than converting the raw value by hand. Prefer it whenever you care about the actual voltage.

3. `Serial.begin(9600)` — starts serial communication at 9600 baud so the values show up in the Serial Monitor.

4. `delay(100)` — one reading every 0.1 s, so the output doesn't scroll too fast to read.

### Serial output format (Monitor + Plotter)

```
ADC_Value:2048,Voltage_mV:1650
```

The layout is deliberate:

- **`label:value`** — a colon separates a label from its value, so the Serial Plotter can name each curve in its legend.
- **`,`** — the separator between two data items (a comma, space, or tab all work).
- **newline** (from `Serial.println()`) — marks the *end of one record*, telling the plotter it can draw the line.

Open **Tools → Serial Plotter** in the Arduino IDE to see two live curves (raw ADC value and millivolts); open the **Serial Monitor** to read the numbers as text.

### ⚠️ Why the maximum reading isn't exactly 3.3V

You may notice `analogRead()` hits **4095 before** the input reaches 3.3V. That's by design: the ESP32 uses an internal **attenuator** to stretch the measurable range, and Arduino enables the largest setting (`ADC_ATTEN_DB_11`) by default. Per Espressif's documentation the reliable upper limit on the **ESP32-S3 is ≈ 3.1V** — above that the reading *saturates* and stays pinned at 4095.

For most 0–100 % style readings this is fine. If you need precision in a specific range, change the attenuation with [`analogSetAttenuation()`](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/adc.html#analogsetattenuation). For accurate voltage readings in general, use `analogReadMilliVolts()` — it is calibrated.

### ⚠️ Reducing noise

With the knob held still, the reading often jitters a little (sometimes with sharp spikes) instead of sitting perfectly still — the ESP32 ADC is sensitive to power-supply noise and electromagnetic interference. Two standard fixes:

- **Hardware:** place a small bypass capacitor (e.g. **100nF** ceramic) between the ADC input pin and GND to shunt high-frequency noise.
- **Software:** average several samples, e.g. read the ADC 10 times in a loop, sum them, divide by 10 — a much smoother value.

For low-precision applications the small fluctuation can simply be ignored.

### ⚠️ Important: the sketch must be in a folder with the same name

The Arduino IDE only accepts a sketch file inside a folder that has **exactly the same name** as the `.ino` file:

```
Analog_Input/
└── Analog_Input.ino    ← folder name and file name must match
```

If you copy `Analog_Input.ino` somewhere else (for example straight into the repository root, or into a folder with a different name), the IDE will complain that the sketch file is not in the correctly named folder. Either open it from the existing `Analog_Input` folder, or create a new folder named `Analog_Input` (matching the file name) and put the `.ino` file inside it.

### Upload

1. Open `Analog_Input/Analog_Input.ino` in the Arduino IDE.
2. Select **Tools → Board → esp32 → ESP32S3 Dev Module** and your COM port.
3. Upload, then open the Serial Monitor at **9600 baud**.
4. Turn the potentiometer — `ADC_Value` sweeps from 0 to 4095 and `Voltage_mV` follows.

**Running result:** the serial monitor shows continuously updating values; the value is 0 at one end of the knob's travel and 4095 at the other.

---

## Ideas to extend it

- Use the potentiometer reading to control something else — e.g. PWM LED brightness (see [Section 5 — PWM Output](https://docs.waveshare.com/ESP32-Arduino-Tutorials/PWM)) or an OLED bar-graph display.
- Add software averaging (the 10-sample mean described above) and compare the stability with/without filtering.
- Map the raw value onto a real-world range with `map()` (0–4095 → 0–100 %) or `constrain()`.

---

## References

- [Waveshare — Analog Input (ESP32 Arduino Tutorials)](https://docs.waveshare.com/ESP32-Arduino-Tutorials/Analog-Input)
- [Waveshare ESP32-S3-Zero](https://www.waveshare.com/esp32-s3-zero.htm)
- Arduino-ESP32: [`ADC` API](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/adc.html) (`analogRead()`, `analogReadMilliVolts()`, `analogSetAttenuation()`)
- Arduino reference: [`analogRead()`](https://docs.arduino.cc/language-reference/en/functions/analog-io/analogRead/)
- [Using the Serial Plotter tool](https://docs.arduino.cc/software/ide-v2/tutorials/ide-v2-serial-plotter/)
- ESP-IDF: [ADC Calibration Driver](https://docs.espressif.com/projects/esp-idf/en/v5.5.1/esp32/api-reference/peripherals/adc_calibration.html#minimize-noise)
- [Comparing ADC Performance of Espressif SoCs](https://developer.espressif.com/blog/2025/08/adc-performance)

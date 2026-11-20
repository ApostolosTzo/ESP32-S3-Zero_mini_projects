# ESP32-S3-Zero Mini Projects

Small, self-contained Arduino projects for the **Waveshare ESP32-S3-Zero** mini development board.

Based on the Waveshare tutorial: [Section 5 — PWM Output](https://docs.waveshare.com/ESP32-Arduino-Tutorials/PWM)

---

## Projects

| Project | Description |
| --- | --- |
| [`sketch_nov20a`](sketch_nov20a/sketch_nov20a.ino) | Turn a potentiometer on GPIO8 → the LED on GPIO7 fades from off to full brightness (PWM) |

---

## sketch_nov20a

Turn the potentiometer knob and the LED smoothly fades brighter and dimmer — the knob position *is* the brightness. This is the **extension exercise** of the Waveshare PWM tutorial: it combines the [ADC analog input](../Analog_Input/Analog_Input_README.md) project with the PWM output project.

### What is PWM?

**PWM (Pulse Width Modulation)** is a way to *simulate* a varying voltage using a purely digital signal. The pin still only ever outputs HIGH (3.3V) or LOW (0V), but it switches between them so fast that the average voltage seen by the load sits somewhere in between.

Two things define a PWM signal:

- **Duty cycle** — the percentage of each period that the signal stays HIGH. It directly sets the average power delivered:
  - **0 %** → ≈ 0 V (LED off)
  - **25 %** → ≈ 0.825 V
  - **50 %** → ≈ 1.65 V
  - **75 %** → ≈ 2.475 V
  - **100 %** → ≈ 3.3 V (LED full brightness)
- **Frequency** — how many full cycles occur per second, in Hz. It must be high enough that the eye/ear can't perceive the switching (LED dimming is fine at a few kHz); for motors it also affects efficiency and audible noise.

### PWM on the ESP32: the LEDC peripheral

The ESP32 has a dedicated hardware block for this — **LEDC (LED Control)**. Despite the name it is a general-purpose PWM generator with **6–16 independent channels** covering **1 Hz – 40 MHz**. (A second block, MCPWM, is aimed at motor control and is not exposed by the standard Arduino core.)

The Arduino ESP32 core gives you two ways to use it:

| Approach | Functions | When to use |
| --- | --- | --- |
| **Simple** | `analogWrite(pin, value)` | You just want a duty cycle — channel allocation, frequency and resolution are chosen for you (default **8-bit**, **5 kHz**) |
| **Detailed** | `ledcAttach()` / `ledcAttachChannel()` + `ledcWrite()` | You need to pick the frequency, the bit resolution, or a specific channel |

This sketch uses the simple `analogWrite()` route; the LEDC API version is shown [below](#ledc-api-version).

### Wiring

| Component | ESP32-S3-Zero pin |
| --- | --- |
| LED anode (+), via a 330Ω resistor | **GPIO7** |
| LED cathode (−) | **GND** |
| Potentiometer VCC (one outer leg) | **3.3V** |
| Potentiometer GND (other outer leg) | **GND** |
| Potentiometer signal (middle leg) | **GPIO8** |

**Components required:** 1× LED, 1× 330Ω resistor, 1× potentiometer, breadboard, jumper wires, ESP32-S3-Zero board.

- The **330Ω resistor limits the current** through the LED (anything between 220Ω and 1kΩ is fine).
- **LED polarity:** long leg (anode) → resistor → GPIO7, short leg (cathode) → GND.
- **Potentiometer** acts as a voltage divider: its middle pin outputs 0–3.3V as the knob turns. GPIO8 is an **ADC1** pin on the ESP32-S3, so it can read that voltage; no `pinMode()` is needed for either pin.

### Code

```cpp
const int ledPin = 7;            // Pin for LED connection
const int potentiometerPin = 8;  // Pin for potentiometer connection

int potentiometerValue;  // Store analog reading value
int brightness;          // Store the mapped brightness value

void setup() {
}

void loop() {
  // 1. Read the analog value of the potentiometer (range 0-4095)
  potentiometerValue = analogRead(potentiometerPin);

  // 2. Map the ADC reading to the PWM range (0-255)
  brightness = map(potentiometerValue, 0, 4095, 0, 255);

  // 3. Set LED brightness with the mapped value
  analogWrite(ledPin, brightness);

  delay(20);
}
```

### How it works

The loop is three steps repeated 50 times a second:

1. **`analogRead(potentiometerPin)`** — the 12-bit ADC converts the wiper voltage into **0–4095** (0V → 0, 3.3V → 4095).
2. **`map(potentiometerValue, 0, 4095, 0, 255)`** — linearly rescales that range onto the PWM duty-cycle range. `map()` does the maths `out = (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin` for you, so knob position ↔ brightness stays proportional.
3. **`analogWrite(ledPin, brightness)`** — applies the duty cycle to GPIO7. The ESP32 core allocates an LEDC channel automatically and uses the default **8-bit / 5 kHz** settings, so `pinMode()` and any LEDC setup are unnecessary.

`delay(20)` paces the loop so the fade feels smooth rather than the knob "jumping" the brightness instantly.

**Signal chain:** `knob → 0–3.3V → ADC 0–4095 → map → 0–255 → PWM duty cycle → average voltage → LED brightness`.

### LEDC API version

If you want to control the frequency and resolution explicitly, the same effect with the lower-level API:

```cpp
const int ledPin = 7;            // GPIO pin for LED connection

const int frequency = 5000;      // PWM frequency 5kHz
const int resolution = 8;        // 8-bit resolution (0-255)
const int ledChannel = 0;        // PWM channel number

void setup() {
  // Initialize the LED PWM function
  ledcAttach(ledPin, frequency, resolution);

  // If you want to specify the channel:
  // ledcAttachChannel(ledPin, frequency, resolution, ledChannel);
}

void loop() {
  int dutyCycle = map(analogRead(8), 0, 4095, 0, 255);
  ledcWrite(ledPin, dutyCycle);
  delay(20);
}
```

- `ledcAttach(pin, frequency, resolution)` — claims the pin for PWM and sets its frequency (Hz) and duty-cycle precision (bits).
- `ledcWrite(pin, duty)` — writes the duty cycle; its range comes from the resolution (**0–255** at 8 bits, **0–1023** at 10 bits, …).
- Higher resolution = finer steps; 8 bits gives 256 brightness levels, which is plenty for an LED.

### ⚠️ The sketch must be in a folder with the same name

The Arduino IDE only accepts a `.ino` file inside a folder with **exactly the same name** as the file:

```
PWD Output/
└── sketch_nov20a/
    └── sketch_nov20a.ino    ← folder name and file name must match
```

Because the folder here is named `PWD Output` (with a space) and the sketch is `sketch_nov20a`, the `.ino` lives one level down in its own correctly named subfolder. Rename the sketch (and its folder together) if you prefer something more descriptive, e.g. `PWM_Pot_LED/PWM_Pot_LED.ino`.

### Upload

1. Open `PWD Output/sketch_nov20a/sketch_nov20a.ino` in the Arduino IDE.
2. Select **Tools → Board → esp32 → ESP32S3 Dev Module** and your COM port.
3. Upload — no Serial Monitor needed for this one.
4. Turn the potentiometer: the LED fades from **off** (fully counter-clockwise) to **full brightness** (fully clockwise).

**Running result:** brightness follows the knob continuously in both directions, with no visible flicker.

---

## Ideas to extend it

- Drive the PWM from `analogWrite()` with an explicit `ledcSetup()`/`ledcAttachChannel()` pair and try different frequencies (e.g. 1 kHz vs 20 kHz) — listen/look for the difference.
- Add a second LED on another channel and map the *same* potentiometer to two different pins.
- Use `constrain()` to limit the duty cycle (e.g. 10–90 %) so the LED never fully turns off/on.
- Swap the LED for a small DC motor (with a driver transistor — never drive a motor directly from a GPIO) to control speed instead of brightness.

---

## References

- [Waveshare — PWM Output (ESP32 Arduino Tutorials)](https://docs.waveshare.com/ESP32-Arduino-Tutorials/PWM)
- [Waveshare ESP32-S3-Zero](https://www.waveshare.com/esp32-s3-zero.htm)
- Arduino-ESP32: [`analogWrite()` / LEDC API](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/ledc.html)
- Arduino reference: [`map()`](https://docs.arduino.cc/language-reference/en/functions/math/map/)
- Arduino reference: [`analogWrite()`](https://docs.arduino.cc/language-reference/en/functions/analog-io/analogWrite/)
- Companion project: [`Analog_Input`](../Analog_Input/Analog_Input_README.md) — reading the potentiometer over Serial

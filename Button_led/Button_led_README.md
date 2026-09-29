# ESP32-S3-Zero Mini Projects

Small, self-contained Arduino projects for the **Waveshare ESP32-S3-Zero** mini development board.

Based on the Waveshare tutorial: [Section 3 — GPIO Digital Output / Input](https://docs.waveshare.com/ESP32-Arduino-Tutorials/Digital-IO#22-code)

---

## Projects

| Project | Description |
| --- | --- |
| [`Button_led`](Button_led/Button_led.ino) | Press a button on GPIO8 → LED on GPIO7 lights up |

---

## Button_led

A button controls an LED: while the button is held down the LED is on, when it is released the LED turns off.

### Wiring

| Component | ESP32-S3-Zero pin |
| --- | --- |
| LED anode (+), via a 330Ω resistor | **GPIO7** |
| LED cathode (−) | **GND** |
| Button (one side) | **GPIO8** |
| Button (other side) | **GND** |

**Components required:** 1× LED, 1× 330Ω resistor, 1× button, breadboard, jumper wires.

- **Current path:** when GPIO7 outputs a high level (3.3V), current flows out of the pin → through the 330Ω resistor → through the LED → back to GND, forming a complete loop.
- **The resistor is a current-limiting resistor** — it protects both the LED and the GPIO pin from excessive current. Any value between **220Ω and 1kΩ** works if you don't have a 330Ω.
- **LED polarity:** long leg (anode) → resistor, short leg (cathode) → GND. A reversed LED simply won't light up, it won't be damaged.
- **Button:** connected between GPIO8 and GND. No external resistor needed — the code uses the ESP32's internal pull-up.

### Code

```cpp
const int buttonPin = 8;  // Define the pin connected to the button
const int ledPin = 7;

int buttonState;

void setup() {
  pinMode(ledPin, OUTPUT);           // Initialize the pin to output mode
  pinMode(buttonPin, INPUT_PULLUP);  // Set the button pin to pull-up input mode
}

void loop() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    digitalWrite(ledPin, HIGH);
  }
  else if (buttonState == HIGH) {
    digitalWrite(ledPin, LOW);
  }
}
```

### How it works

**Digital signals** are binary — they are always in one of two states:

- **HIGH** — logical "1" / true. On the ESP32-S3-Zero this is close to **3.3V** (HIGH always equals the board's operating voltage).
- **LOW** — logical "0" / false, about **0V** (GND).

**Key functions:**

1. `const int buttonPin = 8;`
   - Declaring the pin as `const` means the value never changes at runtime, and the pin number only has to be edited in one place.

2. `pinMode(ledPin, OUTPUT);`
   - Configures the pin's working mode. Must be called before using a digital pin.
   - `OUTPUT`: the ESP32 drives the pin high or low.

3. `pinMode(buttonPin, INPUT_PULLUP);`
   - `INPUT_PULLUP` enables a pull-up resistor *inside* the GPIO, so the pin reads HIGH when the button is open and LOW when the button connects it to GND.
   - This removes the need for an external 10kΩ pull-up resistor — fewer parts, simpler wiring.

4. `digitalWrite(ledPin, HIGH)` / `digitalWrite(ledPin, LOW)`
   - `HIGH` → 3.3V → LED on.
   - `LOW` → 0V → LED off.

5. `digitalRead(buttonPin)`
   - Reads the level of a digital pin and returns `HIGH` (1) or `LOW` (0).
   - Because of `INPUT_PULLUP`: **not pressed → HIGH**, **pressed → LOW** (inverted logic!).

6. `if (buttonState == LOW)` — since the button is active-low, the LED is turned on when the read value is `LOW`.

### ⚠️ Important: the sketch must be in a folder with the same name

The Arduino IDE only accepts a sketch file inside a folder that has **exactly the same name** as the `.ino` file:

```
Button_led/
└── Button_led.ino    ← folder name and file name must match
```

If you copy `Button_led.ino` somewhere else (for example straight into the repository root, or into a folder with a different name), the IDE will complain that the sketch file is not in the correctly named folder. Either open it from the existing `Button_led` folder, or create a new folder named `Button_led` (matching the file name) and put the `.ino` file inside it.

### Upload

1. Open `Button_led/Button_led.ino` in the Arduino IDE.
2. Select **Tools → Board → esp32 → ESP32S3 Dev Module** and your COM port.
3. Upload, then press the button — the LED follows the button state.

---

## Button_led: toggle version (press once → LED stays on)

The first version only lights the LED *while* the button is held. It is also possible to make each press **toggle** the LED — press once to turn it on, press again to turn it off. This is the second extension exercise from the [Waveshare tutorial](https://docs.waveshare.com/ESP32-Arduino-Tutorials/Digital-IO#digital-io-exercise), combined with a simple debounce.

```cpp
const int ledPin = 7;  // Pin number for LED connection
const int buttonPin = 8;  // Pin number for button connection

int lastButtonState = HIGH;  // Last button status
int ledState = LOW;          // Current LED status (LOW = off, HIGH = on)
int currentButtonState;   // Current button status

void setup() {
  pinMode(ledPin, OUTPUT);           // Set the LED pin to output mode
  pinMode(buttonPin, INPUT_PULLUP);  // Set the button pin to pull-up input mode
}

void loop() {
  currentButtonState = digitalRead(buttonPin);  // Read the current state of the button

  // Detect the moment the button changes from pressed to released
  if (lastButtonState == LOW && currentButtonState == HIGH) {
    ledState = !ledState;           // Switch LED status (on to off, off to on)
    digitalWrite(ledPin, ledState); // Apply the new LED status
    delay(100);                     // Debounce delay
  }

  lastButtonState = currentButtonState;  // Save the current state for the next comparison
}
```

**How it works:**

1. `int ledState = LOW;` — the LED's own state is stored in a variable, so it **stays where it was** after the button is released. This is what makes the LED "remember" whether it is on or off.
2. `lastButtonState` / `currentButtonState` — keeping the previous reading lets you detect the *moment* the state changes, instead of reacting continuously to the level.
3. `lastButtonState == LOW && currentButtonState == HIGH` — this is the **release edge**: the pin goes from pressed (LOW, thanks to `INPUT_PULLUP`) back to released (HIGH). Counting on release is what makes the toggle feel natural.
4. `ledState = !ledState;` — the `!` (NOT) operator flips the value: `LOW` → `HIGH`, `HIGH` → `LOW`. One press = one toggle.
5. `delay(100);` — **debounce**: a mechanical button bounces for a few milliseconds on release, which the fast ESP32 would otherwise read as several presses and toggle the LED multiple times. Ignoring everything for 100 ms swallows the bounces.

**Running result:** upload the sketch, open nothing but the board itself — press the button once, the LED turns on and *stays* on; press it again, it turns off.

---

## Ideas to extend it

From the tutorial's extension exercises:

- ~~Toggle the LED state once for every button press~~ — done, see the [toggle version](#button_led-toggle-version-press-once--led-stays-on) above (with `delay(100)` debounce).
- Print the button state to the Serial Monitor with `Serial.println()`.

---

## References

- [Waveshare — Digital Output/Input (ESP32 Arduino Tutorials)](https://docs.waveshare.com/ESP32-Arduino-Tutorials/Digital-IO#22-code)
- [Waveshare ESP32-S3-Zero](https://www.waveshare.com/esp32-s3-zero.htm)
- Arduino reference: [`pinMode()`](https://docs.arduino.cc/language-reference/en/functions/digital-io/pinMode/), [`digitalWrite()`](https://docs.arduino.cc/language-reference/en/functions/digital-io/digitalwrite/), [`digitalRead()`](https://docs.arduino.cc/language-reference/en/functions/digital-io/digitalread/), [`INPUT_PULLUP`](https://docs.arduino.cc/language-reference/en/variables/constants/inputOutputPullup/)

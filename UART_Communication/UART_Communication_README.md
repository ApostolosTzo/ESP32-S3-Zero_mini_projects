# ESP32-S3-Zero Mini Projects

Small, self-contained Arduino projects for the **Waveshare ESP32-S3-Zero** mini development board.

Based on the Waveshare tutorial: [Section 6 — UART Communication](https://docs.waveshare.com/ESP32-Arduino-Tutorials/UART-Communication)

---

## Projects

| Project | Description |
| --- | --- |
| [`UART_Communication`](UART_Communication.ino) | Send `on`/`off` from the Serial Monitor to switch an LED on GPIO7 (Example 1) |
| Two ESP32s over `Serial1` | A button on board A drives an LED on board B over a UART link (Example 2) |

---

## What is UART?

**UART (Universal Asynchronous Receiver/Transmitter)** is a hardware circuit for **asynchronous serial communication**. Typical uses: talking to sensors/modules, and sending logs/debug output to the computer.

Its defining traits:

- **Asynchronous** — no shared clock line. Both sides sync up through a pre-agreed **baud rate** instead.
- **Serial** — bits are sent one after another, not in parallel.
- **Full-duplex** — both directions can transmit at the same time.

Because there is no clock, both sides must agree on the **baud rate** (bits per second — commonly **9600** or **115200**) and the **data frame format**.

Each UART data frame looks like this:

```
 ──┬──┬─────────────┬───┬──
   │  │  Data Bits  │   │
 Start  (5–9 bits,   Parity  Stop
 Bit    usually 8)   Bit     Bits
  0     actual data  (opt.)   1
 ──┴──┴─────────────┴───┴──
```

- **Start bit** — always `0`, signals the beginning of a frame
- **Data bits** — usually 8, the actual payload
- **Parity bit** — optional error detection
- **Stop bits** — always `1`, signals the end of a frame

### UART on the ESP32

The ESP32 has **two or more independent UART controllers**, each configurable for baud rate, data bits, bit order, stop bits and parity. In Arduino they appear as serial objects:

| Object | Typical role |
| --- | --- |
| `Serial` | Default port — talks to the computer (USB-to-serial chip, or native USB when *USB CDC on boot* is enabled). Used for debug prints and uploading code. |
| `Serial1` | An extra hardware UART that can be **mapped to almost any free GPIO**, for talking to external devices |
| `Serial2` | Available on chips with a third UART (the ESP32-S3 has one) |

This is the key trick: `Serial` can keep printing debug info to your computer while `Serial1` talks to another device — **without the two interfering with each other**.

> **Note on `Serial` and native USB:** boards with native USB (ESP32-S3/S2/C3) can point `Serial` at the USB CDC virtual port instead of UART0. When USB CDC is active the baud rate setting is effectively *ignored* (USB is packet-based, not bit-timed), which is why the monitor still works even if the rates don't match. On boards with a separate USB-to-serial chip the baud rate **must** match exactly.

### Wiring principle: cross the TX/RX pairs

UART needs only three signals between the two devices:

- **TX** (transmit), **RX** (receive), **GND** (common ground — the reference both sides measure against)

**The connections are crossed:**

| Device A | Device B | Purpose |
| --- | --- | --- |
| TX | RX | A → B data |
| RX | TX | B → A data |
| GND | GND | **Common ground is required** — without it the voltage levels have no shared reference and the data is garbage |

---

## Example 1: Controlling an LED from the Serial Monitor

The sketch in [`UART_Communication.ino`](UART_Communication.ino) — you type `on` or `off` in the Arduino IDE Serial Monitor and the LED on GPIO7 follows.

### Components

1× LED, 1× 330Ω resistor, breadboard, jumper wires, ESP32-S3-Zero board.

### Wiring

| Component | ESP32-S3-Zero pin |
| --- | --- |
| LED anode (+), via a 330Ω resistor | **GPIO7** |
| LED cathode (−) | **GND** |

### Code

```cpp
const int ledPin = 7;  // Define the pins for the LED connection

void setup() {
  pinMode(ledPin, OUTPUT);           // Set the LED pin to output mode

  Serial.begin(115200);                // Initialize serial communication and set the baud rate to 115200
  while (!Serial) {};    // Wait for the serial port to be ready
}

void loop() {
  if (Serial.available() > 0) {                 // If the serial port receives data
    String msg = Serial.readStringUntil('\n');  // Read a line of input (ending with a line break)
    msg.trim();                                 // Remove leading and trailing invisible characters that take up space, such as spaces and newline characters

    if (msg == "on") {
      // If the input is "on", turn on the LED
      digitalWrite(ledPin, HIGH);
      Serial.println("LED is on");
    } else if (msg == "off") {
      // If the input is "off", turn off the LED
      digitalWrite(ledPin, LOW);
      Serial.println("LED is off");
    }else{
      Serial.println("Please enter 'on' or 'off'");  // Prompt for valid input
    }
  }
}
```

### How it works

1. **`Serial.begin(115200)`** — starts `Serial` at 115200 baud. The Serial Monitor must be set to the **same** baud rate (unless USB CDC is enabled, in which case it's ignored).
2. **`Serial.available() > 0`** — checks whether bytes have arrived in the receive buffer.
3. **`Serial.readStringUntil('\n')`** — reads characters until a newline (or timeout) and bundles them into a `String`.
4. **`msg.trim()`** — pressing Enter also sends `\n`/`\r` (and the monitor may add spaces); `trim()` strips them so `msg == "on"` can actually match.
5. **`if / else if`** — performs the `digitalWrite()` and prints feedback back over `Serial`.

### Running result

1. Open the Serial Monitor at **115200** baud.
2. Type `on` + Enter → LED turns **on**, monitor prints `LED is on`.
3. Type `off` + Enter → LED turns **off**, monitor prints `LED is off`.
4. Anything else → `Please enter 'on' or 'off'`.

---

## Example 2: Two ESP32s Talking to Each Other over UART

This is the two-board part of the tutorial: **board A has a button, board B has an LED**, and pressing the button on A lights (and releasing it extinguishes) the LED on B — data crossing between the two boards on a `Serial1` UART link.

### Components

- 2× ESP32 development board (ESP32-S3-Zero)
- 1× LED + 1× 330Ω resistor (on board B)
- 1× Button (on board A)
- 2× breadboard, jumper wires

### Wiring

**Board A (transmitter, has the button):**

| Component | Board A pin |
| --- | --- |
| Button leg 1 | **GPIO7** |
| Button leg 2 | **GND** (uses the internal pull-up) |

**Board B (receiver, has the LED):**

| Component | Board B pin |
| --- | --- |
| LED anode (+), via 330Ω resistor | **GPIO7** |
| LED cathode (−) | **GND** |

**UART link between the two boards (crossed!):**

| Board A | Board B | Direction |
| --- | --- | --- |
| GPIO12 (TX) | GPIO1 (RX) | Data sent from **A → B** |
| GPIO11 (RX) | GPIO2 (TX) | Data sent from **B → A** |
| GND | GND | **Common ground required** for stable signals |

> Both boards can be plugged into your computer with two USB cables — one cable per board — so each one can print debug info to its own Serial Monitor while `Serial1` carries the button/LED traffic between them.

### Transmitter code (board A — the one with the button)

```cpp
#define UART1_RX_PIN 11  // Define the receive pin (RX) of UART1
#define UART1_TX_PIN 12  // Define the transmit pin (TX) of UART1

const int buttonPin = 7;    // Pin for button connection
int lastButtonState = LOW;  // The initial state is not pressed

void setup() {
  // Start the default serial port for debugging output to the computer
  Serial.begin(115200);
  // while(!Serial){};

  // Start Serial1 and specify RX and TX pins for device communication
  Serial1.begin(9600, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);

  pinMode(buttonPin, INPUT_PULLUP);  // Configure button pins to pull-up input mode

  Serial.println("Sender Ready. Press the button.");
}

void loop() {
  int currentButtonState = digitalRead(buttonPin);

  // If the button status changes
  if (currentButtonState != lastButtonState) {
    if (currentButtonState == HIGH) {
      Serial1.write('0');  // Send '0' when the button is released
      Serial.println("Sent: 0 (Button Released)");
    }else{
      Serial1.write('1');  // Send '1' when the button is pressed
      Serial.println("Sent: 1 (Button Pressed)");
    }

    lastButtonState = currentButtonState;  // Update button status
    delay(50);                             // Simple debouncing
  }
}
```

### Receiver code (board B — the one with the LED)

```cpp
#define UART1_RX_PIN 1  // Define the receive pin (RX) of UART1
#define UART1_TX_PIN 2  // Define the transmit pin (TX) of UART1

const int ledPin = 7;

void setup() {
  // Start the default serial port for debugging output to the computer
  Serial.begin(115200);
  // while(!Serial){};

  // Start Serial1 and specify RX and TX pins for device communication
  Serial1.begin(9600, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);

  pinMode(ledPin, OUTPUT);  // Configure LED pin to output mode

  Serial.println("Receiver Ready. Waiting for commands...");
}

void loop() {
  // Check if data is received from UART1 serial port
  if (Serial1.available()) {
    char command = Serial1.read();  // Read a byte (character)

    // Control the LED based on the received command
    if (command == '1') {
      // Light the LED when receiving '1'
      digitalWrite(ledPin, HIGH);
      Serial.println("Received: 1 -> LED ON");
    } else if (command == '0') {
      // Turn off the LED when receiving '0'
      digitalWrite(ledPin, LOW);
      Serial.println("Received: 0 -> LED OFF");
    }
  }
}
```

### How it works

**Shared setup — both boards:**

- `#define UART1_RX_PIN ...` / `#define UART1_TX_PIN ...` — macros keep the pin assignments readable and easy to change in one place.
- `Serial.begin(115200)` — the *debug* port to the computer. Independent of the link between the boards.
- **`Serial1.begin(9600, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN)`** — the heart of the example:
  - **`9600`** — the baud rate of the board-to-board link. **Must be identical on both sides.**
  - **`SERIAL_8N1`** — the frame format: **8** data bits, **N**o parity, **1** stop bit (the Arduino default: 8E = even parity, 8O = odd parity; `2` would mean 2 stop bits).
  - **`UART1_RX_PIN, UART1_TX_PIN`** — binds `Serial1` to the chosen GPIOs, so the UART isn't locked to fixed pins.

**Transmitter (board A):**

- `if (currentButtonState != lastButtonState)` — only reacts on a **state change** (press or release), so one press sends exactly one pair of messages instead of a continuous stream.
- `Serial1.write('1')` — button pressed (pin goes LOW with the pull-up) → send the character `'1'`.
- `Serial1.write('0')` — button released (pin back HIGH) → send `'0'`.
- `delay(50)` — crude but effective **debounce**; mechanical buttons bounce for a few milliseconds after contact.

**Receiver (board B):**

- `if (Serial1.available())` — polls the `Serial1` receive buffer each loop.
- `Serial1.read()` — pulls one byte out.
- `command == '1'` → LED on, `command == '0'` → LED off; each action is echoed to board B's own `Serial` monitor for debugging.

### Running result

1. Upload the transmitter sketch to **board A** and the receiver sketch to **board B** (two boards = two uploads; select the matching COM port each time).
2. Connect both boards to the computer and open **two** Serial Monitors, one per COM port.
3. **Press** the button on board A:
   - A's monitor prints `Sent: 1 (Button Pressed)`
   - the **LED on board B lights up**
   - B's monitor prints `Received: 1 -> LED ON`
4. **Release** the button:
   - A's monitor prints `Sent: 0 (Button Released)`
   - the **LED on board B turns off**
   - B's monitor prints `Received: 0 -> LED OFF`

### Troubleshooting

- **Nothing arrives** — check the TX/RX crossing (A-TX → B-RX, A-RX → B-TX). A straight-through cable is the most common mistake.
- **Garbage characters** — the two boards must agree on baud rate *and* frame format (`9600, SERIAL_8N1` on both), and **GND must be connected** between the boards.
- **LED stays on / flickers** — verify the button is on GPIO7 with `INPUT_PULLUP` and wired to GND, and that the 50 ms debounce delay is present.

---

## Ideas to extend it

- Send multi-byte messages instead of single characters — e.g. `Serial1.print("temperature=23.5\n")` — and parse lines on the receiver with `readStringUntil('\n')` (the same trick Example 1 uses).
- Give board B a reply channel: have it `Serial1.write('a')` as an acknowledgement and have board A wait for it before sending again.
- Raise the baud rate (`115200`) on both sides and send a counter to measure the throughput.
- Swap the button for a sensor on board A and stream readings to board B, which displays them on an LCD/OLED.
- Add a parity bit (`SERIAL_8E1`) to see basic error detection in action.

---

## References

- [Waveshare — UART Communication (ESP32 Arduino Tutorials)](https://docs.waveshare.com/ESP32-Arduino-Tutorials/UART-Communication)
- [Waveshare ESP32-S3-Zero](https://www.waveshare.com/esp32-s3-zero.htm)
- Arduino reference: [`Serial`](https://docs.arduino.cc/language-reference/en/functions/communication/serial/), [`Serial.write()`](https://docs.arduino.cc/language-reference/en/functions/communication/serial/write/), [`Serial.readStringUntil()`](https://docs.arduino.cc/language-reference/en/functions/communication/stream/readstringuntil/)
- Arduino docs: [UART communication](https://docs.arduino.cc/learn/communication/uart/)
- Arduino-ESP32: [USB CDC](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/usb_cdc.html) (why baud rate can be ignored over native USB)

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
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
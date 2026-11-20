const int ledPin = 7;            // Pin for LED connection
const int potentiometerPin = 8;  // Pin for potentiometer connection

int potentiometerValue;  // Store potentiometer reading value
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

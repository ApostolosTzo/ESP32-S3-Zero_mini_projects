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
    digitalWrite(ledPin,LOW);
    
  }
}
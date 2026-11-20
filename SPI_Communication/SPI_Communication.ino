#include <SPI.h>
#include <Adafruit_SSD1327.h>

#define OLED_SCK 13    // Clock cable
#define OLED_MOSI 11   // Data output line
#define OLED_CS 10     // Chip select line

#define OLED_DC 8      // Data/command select line
#define OLED_RESET -1  // Reset pin (unused)

// Create a display object
Adafruit_SSD1327 display(128, 128, &SPI, OLED_DC, OLED_RESET, OLED_CS);

void setup() {

  Serial.begin(9600);

  // Initialize SPI bus
  SPI.begin(OLED_SCK, -1, OLED_MOSI, OLED_CS);

  Serial.println("SSD1327 OLED test");

  // Initialize the display
  if (!display.begin()) {
    Serial.println("Unable to initialize OLED");
    while (1) yield();
  }

  // Display settings
  display.clearDisplay();
  display.setRotation(3);
  display.setTextSize(2);
  display.setTextColor(SSD1327_WHITE);

  // Display text
  display.setCursor(10, 10);
  display.println("Hello,");
  display.setCursor(40, 30);
  display.setTextColor(SSD1327_BLACK, SSD1327_WHITE);
  display.println(" World!");
  display.display();
  display.setCursor(15, 60);
  display.setTextColor(SSD1327_WHITE);
  display.println("SPI TEST");
  display.display();

  delay(1000);
}

void loop() {
}
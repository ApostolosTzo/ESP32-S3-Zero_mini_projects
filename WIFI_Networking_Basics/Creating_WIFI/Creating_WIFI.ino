#include <WiFi.h>

const char *ssid = "ESP32-S3-TEST";  // Set the hotspot name
const char *password = "12345678";  // Set hotspot password (at least 8 digits)

void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // Set Wi-Fi event callback function
  WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED);
  WiFi.onEvent(WiFiStationGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED);
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);

  // Create a Wi-Fi hotspot
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("Soft AP creation failed.");
    while (1)
      ;
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void loop() {
}

// Device connection event
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.print("Device connected, MAC: ");
  Serial.println(macToString(info.wifi_ap_staconnected.mac));
}

// Device obtaining IP event
void WiFiStationGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.print("Device IP address: ");
  Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
}

// Device disconnection event
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.print("Device disconnected, MAC: ");
  Serial.println(macToString(info.wifi_ap_stadisconnected.mac));
}

// MAC address to string conversion helper function
String macToString(const uint8_t *mac) {
  char buf[18];
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}
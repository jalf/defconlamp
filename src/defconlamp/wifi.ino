// Wifi enabled led lamp
// Jalf, November 2023

#include <ESPmDNS.h>

void setupWifi() {
  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  //wifiManager.resetSettings();

  wifiManager.setAPCallback(configModeCallback);
  WiFi.mode(WIFI_STA);
  // Boot config pattern
  FastLED.clear();
  fill_solid(leds, NUM_LEDS, CRGB::Violet);
  FastLED.show();

  // warm-up
  delay(3000);

  // Configure host settings
  char hostName[25];
  uint64_t chipid = ESP.getEfuseMac();
  uint16_t chip = (uint16_t)(chipid >> 32);
  snprintf(hostName, 25, "DEFCONLAMP-%04X%08X", chip, (uint32_t)chipid);
  WiFi.setHostname(hostName);  //define hostname

  // Start wifi portal
  bool res;
  res = wifiManager.autoConnect(hostName);  // anonymous ap

  if (!res) {
    Serial.println("Failed to connect");

    // Set config pattern
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.show();

    delay(1000);
    ESP.restart();
  } else {
    if (MDNS.begin(hostName)) {
      Serial.println("MDNS responder started");
    }

    // Set config pattern
    fill_solid(leds, NUM_LEDS, CRGB::Green);
    FastLED.show();
  }
}

void configModeCallback(WiFiManager *myWiFiManager) {
  // Set config pattern
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
  FastLED.show();
}
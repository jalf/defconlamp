// Wifi enabled led lamp
// Jalf, November 2023

#include <FastLED.h>
FASTLED_USING_NAMESPACE

#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include "time.h"         // for NTP service
#include "DHT_Async.h"    // https://github.com/toannv17/DHT-Sensors-Non-Blocking
#include <WebServer.h>    // https://github.com/espressif/arduino-esp32/tree/master/libraries/WebServer

// LED settings
#define DATA_PIN 25
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 80
CRGB leds[NUM_LEDS];
#define FRAMES_PER_SECOND 120

// NTP settings
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -10800;
const int daylightOffset_sec = 0;
struct tm gTimeinfo;

// Sensors settings
#define LDR_PIN 35
/* Uncomment according to your sensortype. */
#define DHT_SENSOR_TYPE DHT_TYPE_11

#define DHT_SENSOR_PIN 33
DHT_Async dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

//WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
WiFiManager wifiManager;

// Wifi Server
WebServer webServer(80);

void setup() {
  Serial.begin(115200);
  pinMode(LDR_PIN, INPUT);

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(255);

  // Initialize wifi
  setupWifi();

  // Initalize the webServer
  ws_setup();

  // Get current date/time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  getLocalTime(&gTimeinfo);

  delay(3000);  // 3 second delay for recovery*/
}

uint8_t gHue = 0;  // rotating "base color" used by many of the patterns

// User Settings
uint8_t gCurrentPatternNumber = 0;  // Index number of which pattern is current
uint8_t gCycleIndex = 0;            // Index of active cycle patterns
bool gCyclePatterns = true;         // Cycle all patterns
bool gRandonPatterns = false;       // Set the pattern randomically
uint8_t gTimeInSecondsForNextPattern = 30;
CRGB gSolidColor = CRGB::Red;      // Color used in solid mode
CRGB gSolidColor2 = CRGB::Blue;    // End Color used in gradient mode
uint8_t gCurrentBrightness = 255;  // Brightness level
bool gIsOn = true;                 // General on/off state

// Sensor values
float gLightSensorValue = 0;  // Light sensor level
float gTemperature = 0;
float gHumidity = 0;

// List of patterns available.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { solid, gradient,
                                rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, pacifica, fire, pride,
                                cloudTwinkles, rainbowTwinkles, snowTwinkles, incandescentTwinkles,
                                redGreenWhiteTwinkles, hollyTwinkles, redWhiteTwinkles, blueWhiteTwinkles, fairyLightTwinkles, snow2Twinkles, iceTwinkles,
                                retroC9Twinkles, partyTwinkles, forestTwinkles, lavaTwinkles, fireTwinkles, cloud2Twinkles, oceanTwinkles };

// Human-readable pattern names
String gPattern_names[] = { "Solid", "Gradient",
                            "Rainbow", "RainbowWithGlitter", "Confetti", "Sinelon", "Juggle", "Bpm", "Pacifica", "Fire", "Pride",
                            "CloudTwinkles", "RainbowTwinkles", "SnowTwinkles", "IncandescentTwinkles",
                            "RedGreenWhiteTwinkles", "HollyTwinkles", "RedWhiteTwinkles", "BlueWhiteTwinkles", "FairyLightTwinkles", "Snow2Twinkles", "IceTwinkles",
                            "RetroC9Twinkles", "PartyTwinkles", "ForestTwinkles", "LavaTwinkles", "FireTwinkles", "Cloud2Twinkles", "OceanTwinkles" };

// Patterns shown in cycle mode
uint8_t gPatternsInCycle[] = { 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28 };


void loop() {
  if (gIsOn) {
    EVERY_N_MILLISECONDS(1000 / FRAMES_PER_SECOND) {
      // Call the current pattern function once, updating the 'leds' array
      gPatterns[gCurrentPatternNumber]();
      // send the 'leds' array out to the actual LED strip
      FastLED.show();
    }

    // change patterns periodically if needed
    EVERY_N_SECONDS(gTimeInSecondsForNextPattern) {
      checkNewPattern();
    }
  }
  // update sensors
  EVERY_N_SECONDS(1) {
    MeasureLight();
    MeasureTemp();
  }
  // update time
  EVERY_N_SECONDS(1) {
    getLocalTime(&gTimeinfo);
  }

  // Handle webserver
  webServer.handleClient();
  delay(2);  //allow the cpu to switch to other tasks
}

void MeasureTemp() {
  float temperature;
  float humidity;
  if (dht_sensor.measure(&temperature, &humidity)) {
    gTemperature = temperature;
    gHumidity = humidity;
  }
}

void MeasureLight() {
  gLightSensorValue = analogRead(LDR_PIN);
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void checkNewPattern() {
  uint8_t oldPattern = gCurrentPatternNumber;

  if (gCyclePatterns) {
    // add one to the current pattern number, and wrap around at the end
    gCycleIndex = (gCycleIndex + 1) % ARRAY_SIZE(gPatternsInCycle);
    gCurrentPatternNumber = gPatternsInCycle[gCycleIndex];
  }

  if (gRandonPatterns) {
    // get next pattern number
    gCycleIndex = random8(ARRAY_SIZE(gPatternsInCycle));
    gCurrentPatternNumber = gPatternsInCycle[gCycleIndex];
  }

  // The pattern was changed
  if (oldPattern != gCurrentPatternNumber) {
    gHue = 0;
  }
}
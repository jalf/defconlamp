// Wifi enabled led lamp
// Jalf, November 2023

#include <ArduinoJson.h>
#include <uri/UriBraces.h>

void ws_notFound() {
  String message = "Page Not Found\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  for (uint8_t i = 0; i < webServer.args(); i++) {
    message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  }
  webServer.send(404, "text/plain", message);
}

void ws_home() {
  String htmlPage = "<!DOCTYPE html><html><body style=\"font-family:Arial\"><center><svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"48px\" height=\"48px\" ";
  htmlPage += "viewBox=\"0 0 32 32\" xml:space=\"preserve\"><style type=\"text/css\">.avocado_een{fill:#231f20}.avocado_zeven{fill:#788287}.avocado_zeventien{fill:#cc4121}.avocado_achttien{fill:#d1712a}.st0{fill:#231f20}";
  htmlPage += ".st1{fill:#0b1719}.st2{fill:#ead13f}.st3{fill:#e0a838}.st4{fill:#716558}.st5{fill:#fffaee}.st6{fill:#3d3935}.st7{fill:#dbd2c1}.st8{fill:#a3aeb5}.st9{fill:#8d3e93}.st10{fill:#248ebc}.st11{fill:#6f9b45}";
  htmlPage += ".st12{fill:#af9480}.st13{fill:#c3cc6a}</style><g id=\"Uploaded to svgrepo.com\"><g><polygon class=\"avocado_zeven\" points=\"2.93,29.5 3.729,25.5 28.59,25.5 29.39,29.5\"/><path class=\"avocado_zeventien\" ";
  htmlPage += "d=\"M4.862,25.5L6.206,9.377C6.527,5.521,9.811,2.5,13.681,2.5h4.958c3.87,0,7.153,3.021,7.475,6.877L27.457,25.5H4.862z\"/><g><path class=\"avocado_achttien\" d=\"M12.452,25.5l0.885-9.727c0.118-1.296,";
  htmlPage += "1.188-2.273,2.489-2.273h0.348c1.301,0,2.371,0.978,2.489,2.273l0.885,9.727H12.452z\"/></g><path class=\"avocado_een\" d=\"M29,25h-1.083L26.611,9.336C26.266,5.189,22.8,2,18.639,2h-4.958C9.52,2,6.054,";
  htmlPage += "5.189,5.708,9.336L4.403,25H3.319l-1,5H30L29,25z M6.705,9.419C7.005,5.819,10.069,3,13.681,3h4.958c3.612,0,6.676,2.819,6.976,6.419L26.913,25h-6.909l-0.008-0.091l-0.835-9.181C19.02,14.173,17.735,13,16.174,";
  htmlPage += "13h-0.347c-1.562,0-2.846,1.173-2.988,2.728l-0.835,9.181L11.996,25h-6.59L6.705,9.419z M19,25h-6l0.835-9.181C13.928,14.789,14.792,14,15.826,14h0.347c1.034,0,1.898,0.789,1.992,1.819";
  htmlPage += "L19,25z M3.539,29l0.6-3H28.18l0.6,3H3.539z\"/></g></g></svg><h2>DefconLamp</h2><small>An over-engineered RGB lamp controlled by a REST API that can be used to illuminate a room but also as a warning ";
  htmlPage += "device for alarms and events.</small></center><br/><h4>[The API]</h4><ul><li><i>GET</i><b>/status</b>- Get the current status</li><li><i>POST</i><b>/stop</b>- Stop the patterns cycle</li><li><i>POST</i>";
  htmlPage += "<b>/cycle</b>- Start the patterns cycle</li><li><i>POST</i><b>/random</b>- Set an random order to patterns cycle</li><li><i>GET</i><b>/patterns</b>- Get the available patterns list</li><li><i>POST</i>";
  htmlPage += "<b>/pattern/{pattern_name}</b>- Set the active pattern</li><li><i>POST</i><b>/delay/{delay}</b>- Delay (in seconds) to change a pattern in cycle mode</li><li><i>POST</i><b>/on</b>- Turn on the lights</li>";
  htmlPage += "<li><i>POST</i><b>/off</b>- Turn off the lights</li><li><i>POST</i><b>/reset</b>- Reset wifi settings</li><li><i>POST</i><b>/color/{r}/{g}/{b}</b>- Set a solid RGB color</li></ul><br/><h4>[Sensors]</h4>";
  htmlPage += "<p>For no specific reason, the lamp also has <b>temperature</b>, <b>humidity</b> and <b>light</b> sensors. Use them wisely.</p><br><br><center><h5>Brought to you by <a href=\"https://github.com/jalf\">Jalf</a></h5></center></body></html>\n";

  webServer.send(200, "text/html", htmlPage);
}

void ws_status() {
  DynamicJsonDocument statusDoc(255);

  statusDoc["pattern"] = gPattern_names[gCurrentPatternNumber];
  statusDoc["cycle"] = gCyclePatterns;
  statusDoc["random"] = gRandonPatterns;
  statusDoc["brightnessLevel"] = gCurrentBrightness;
  statusDoc["lightSensorValue"] = gLightSensorValue;
  statusDoc["temperature"] = gTemperature;
  statusDoc["humidity"] = gHumidity;
  statusDoc["patternDelayInSeconds"] = gTimeInSecondsForNextPattern;
  statusDoc["lightsOn"] = gIsOn;

  // Add the current time
  char timeStringBuff[50];  //50 chars should be enough
  strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &gTimeinfo);
  statusDoc["clock"] = timeStringBuff;

  char buffer[255];
  serializeJson(statusDoc, buffer);

  // send payload
  webServer.send(200, "application/json", buffer);
}

void ws_stop() {
  if (webServer.method() == HTTP_POST) {
    gCyclePatterns = false;
    gRandonPatterns = false;
    ws_status();
  } else ws_notFound();
}

void ws_cycle() {
  if (webServer.method() == HTTP_POST) {
    gCyclePatterns = true;
    gRandonPatterns = false;
    ws_status();
  } else ws_notFound();
}

void ws_random() {
  if (webServer.method() == HTTP_POST) {
    gCyclePatterns = false;
    gRandonPatterns = true;
    ws_status();
  } else ws_notFound();
}

void ws_pattern() {
  if (webServer.method() == HTTP_POST) {
    for (int i = 0; i < ARRAY_SIZE(gPattern_names); i++) {
      if (gPattern_names[i] == webServer.pathArg(0)) {
        gCurrentPatternNumber = i;
        gCyclePatterns = false;
        gRandonPatterns = false;
        ws_status();
        return;
      }
    }
    // invalid params
    webServer.send(400, "text/plain", "invalid pattern");
  } else ws_notFound();
}

void ws_delay() {
  if (webServer.method() == HTTP_POST) {
    gTimeInSecondsForNextPattern = webServer.pathArg(0).toInt();
    ws_status();
  } else ws_notFound();
}

void ws_patterns() {

  DynamicJsonDocument jsonDoc(1024);

  for (int i = 0; i < ARRAY_SIZE(gPattern_names); i++)
    jsonDoc["pattern"][i] = gPattern_names[i];

  char buffer[1024];
  serializeJson(jsonDoc, buffer);

  // send payload
  webServer.send(200, "application/json", buffer);
}

void ws_turnOn() {
  if (webServer.method() == HTTP_POST) {
    gIsOn = true;
    gCurrentPatternNumber = 0;
    ws_status();
  } else ws_notFound();
}

void ws_turnOff() {
  if (webServer.method() == HTTP_POST) {
    gIsOn = false;

    // clear all pixel data
    FastLED.clear();
    FastLED.show();

    ws_status();
  } else ws_notFound();
}

void ws_reset() {
  if (webServer.method() == HTTP_POST) {

    // clear all pixel data
    FastLED.clear();
    FastLED.show();

    wifiManager.resetSettings();
    ESP.restart();
  } else ws_notFound();
}

void ws_setColor() {
  if (webServer.method() == HTTP_POST) {
    gSolidColor.setRGB(webServer.pathArg(0).toInt(),  // R
                       webServer.pathArg(1).toInt(),  // G
                       webServer.pathArg(2).toInt()); // B
    gCurrentPatternNumber = 0;  // solid pattern
    gCyclePatterns = false;
    ws_status();
  }
  else ws_notFound();
}


// Initialize and configure webserver routes
void ws_setup() {
  webServer.onNotFound(ws_notFound);
  webServer.on("/", ws_home);
  webServer.on("/stop", ws_stop);
  webServer.on("/status", ws_status);
  webServer.on("/cycle", ws_cycle);
  webServer.on("/random", ws_random);
  webServer.on("/patterns", ws_patterns);
  webServer.on("/on", ws_turnOn);
  webServer.on("/off", ws_turnOff);
  webServer.on("/reset", ws_reset);
  webServer.on(UriBraces("/color/{}/{}/{}"), ws_setColor);
  webServer.on(UriBraces("/pattern/{}"), ws_pattern);
  webServer.on(UriBraces("/delay/{}"), ws_delay);

  webServer.begin();

  Serial.println("HTTP server started");
}
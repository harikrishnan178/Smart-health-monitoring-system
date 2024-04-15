#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS     1000
float BPM, SpO2;
/*Put your SSID & Password*/
const char* ssid = "SVCT";  // Enter SSID here
const char* password = "svct@321";  //Enter Password here
PulseOximeter pox;
uint32_t tsLastReport = 0;
WebServer server(80);
void onBeatDetected()
{
  Serial.println("Beat Detected!");
}
void setup() {
  Serial.begin(115200);
  pinMode(19, OUTPUT);
  delay(100);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  //connect to your local wi-fi network
  WiFi.begin(ssid, password);
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
  
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
  
  Serial.print("Initializing pulse oximeter..");
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
    pox.setOnBeatDetectedCallback(onBeatDetected);
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  // Register a callback for the beat detection
}
void loop() {
  server.handleClient();
  pox.update();
  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS)
  {
    //Serial.print("BPM: ");
    //Serial.println(BPM);
    /*Serial.print("SpO2: ");
    Serial.print(SpO2);
    Serial.println("%");
    Serial.println("*********************************");
    Serial.println();*/
    tsLastReport = millis();
  }
}
void handle_OnConnect() {
  server.send(200, "text/html", SendHTML(BPM, SpO2));
}
void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}
String SendHTML(float BPM, float SpO2) {
String ptr = "<!DOCTYPE html>";
//ptr += "<html lang='en'>";
ptr += "<head>";
ptr += "<meta charset='UTF-8'>";
ptr += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
ptr += "<title>My Health monitor</title>";
ptr += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.7.2/css/all.min.css'>";
ptr += "<style>";
ptr += "body { background-color: peachpuff; font-family: sans-serif; color: #333333; font: 14px Helvetica, sans-serif box-sizing : border-box; }";
ptr += "#page { margin: 20px; background-color: peachpuff; }";
ptr += ".container { height: inherit; padding-bottom: 20px; }";
ptr += ".header { padding: 20px; }";
ptr += ".header h1 { padding-bottom: 0.3em; color: purple; font-size: 90px; font-weight: bold;";
ptr += "font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;";
ptr += "text-align: center; }";
ptr += "h2 { padding-bottom: 0.2em; border-bottom: 1px solid #eee; margin: 2px; text-align: left; color: rgb(89, 33, 169);";
ptr += "font-size: 50px; font-family: 'Lucida Sans', 'Lucida Sans Regular', 'Lucida Grande', 'Lucida Sans Unicode', Geneva, Verdana, sans-serif; }";
ptr += ".header h3 { font-weight: bold; font-family: Arial, 'sans-serif'; font-size: 17px; color: #b6b6b6; text-align: center; }";
ptr += ".box-full { padding: 20px; border 1px solid #ddd; border-radius: 1em 1em 1em 1em; box-shadow: 1px 7px 7px 1px rgba(0,0,0,0.4);";
ptr += "background:rgb(105, 246, 152); margin: 20px; width: 500px; position: relative; }";
ptr += "@media (max-width: 494px) { #page { width: inherit; margin: 5px auto; } #content { padding: 1px; }";
ptr += ".box-full { margin: 8px 8px 12px 8px; padding: 10px; width: inherit; float: none; } }";
ptr += "@media (min-width: 494px) and (max-width: 980px) { #page { width: 465px; margin 0 auto; } .box-full { width: 380px; } }";
ptr += "@media (min-width: 980px) { #page { width: 930px; margin: auto; } }";
ptr += ".sensor { margin: 12px 0px; font-size: 2.5rem; position: relative; }";
ptr += ".sensor-labels { font-size: 1rem; vertical-align: middle; padding-bottom: 15px; }";
ptr += ".units { font-size: 1.2rem; }";
ptr += "hr { height: 1px; color: #eee; background-color: #eee; border: none; }";
ptr += ".heartbeat-icon { color: #cc3300; animation: heartbeat 1s infinite; }";
ptr += ".pulse-icon { color: #f7347a; animation: pulse 1s infinite; }";
ptr += "@keyframes heartbeat { 0% { transform: scale(1); } 50% { transform: scale(1.2); } 100% { transform: scale(1); } }";
ptr += "@keyframes pulse { 0% { transform: scale(1); } 50% { transform: scale(1.2); } 100% { transform: scale(1); } }";
ptr += "</style>";
ptr += "</head>";
ptr += "<body>";
ptr += "<div id='page'>";
ptr += "<div class='header'>";
ptr += "<h1>Health Monitor</h1><hr>";
ptr += "</div>";
ptr += "<div id='content' align='center'>";
ptr += "<div class='box-full' align='left'>";
ptr += "<h2>Sensor Readings</h2>";
ptr += "<div class='sensors-container'>";
// For Heart Rate
ptr += "<p class='sensor'>";
ptr += "<i class='fas fa-heartbeat heartbeat-icon'></i>";
ptr += "<span class='sensor-labels'> Heart Rate </span>";
ptr += (int)BPM; // Replace with actual BPM value
ptr += "<sup class='units'>BPM</sup>";
ptr += "</p>";
ptr += "<hr>";
// For SpO2
ptr += "<p class='sensor'>";
ptr += "<i class='fas fa-burn pulse-icon'></i>";
ptr += "<span class='sensor-labels'> SpO2 </span>";
ptr += (int)SpO2; // Replace with actual SpO2 value
ptr += "<sup class='units'>%</sup>";
ptr += "</p>";
ptr += "</div>";
ptr += "</div>";
ptr += "</div>";
ptr += "</div>";
ptr += "</div>";
ptr += "</body>";
ptr += "</html>";

  return ptr;
}
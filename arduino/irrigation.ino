#define LOGGING
#define ARDUINO_DISABLE_ECCX08

#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <WiFi101.h>
#include <ArduinoBearSSL.h>

#include "wifi-secrets.h"
#include "api-secrets.h"
#include "sensor.h"
#include "pump.h"

void(* resetFunc) (void) = 0;

char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)


WiFiClient wifi;
BearSSLClient sslClient(wifi);
HttpClient client = HttpClient(sslClient, API_HOST, 443);
long time = 0;

void printWifiStatus() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print SSID:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.println(rssi);
}


void connectWifi() {
  int status = WL_IDLE_STATUS;  // the Wifi radio's status
  
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to a network: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");

  Serial.println("----------------------------------------");
  printWifiStatus();
  Serial.println("----------------------------------------");
}

unsigned long getTime() {
  return WiFi.getTime();
}

void setup() {
  // put your setup code here, to run once:

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) continue;

  connectWifi();

  ArduinoBearSSL.onGetTime(getTime);

  if (!initSensors(client)) {
    delay(10000);
    resetFunc();
  };
  initPumps(client);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }
  
  uploadMeasurements(client, time);
  time++;
  delay(10000000);
}

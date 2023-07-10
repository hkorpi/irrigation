#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>

#include "sensor.h"
#include "api-secrets.h"
#include "rest-api.h"

struct Sensor {
  int id;
  int interval;
  int pin;
};

static Sensor* sensors;
static int sensorAmount = 0;

void printSensor (Sensor& sensor) {
  Serial.print(sensor.id);
  Serial.print(", ");
  Serial.print(sensor.interval);
  Serial.print(", ");
  Serial.print(sensor.pin);
}

void initSensors(HttpClient& client) {
  Serial.print("Making GET request for sensors: ");
  Serial.println(API_HOST);
  
  client.beginRequest();
  client.get("/rest/v1/sensor?select=*");
  sendAuthorization(client);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("GET status code: ");
  Serial.println(statusCode);
  Serial.print("GET response: ");
  Serial.println(response);

  StaticJsonDocument<500> doc;

  // parse a JSON array
  deserializeJson(doc, response);

  JsonArray json_sensors = doc.as<JsonArray>();
  sensorAmount = json_sensors.size();
  sensors = new Sensor[sensorAmount];

  for(int i = 0; i < sensorAmount; i++) {
    sensors[i].id = (int) json_sensors[i]["id"];
    sensors[i].interval = (int) json_sensors[i]["interval"];
    sensors[i].pin = (int) json_sensors[i]["pin"];
  }

  Serial.println("Found sensors: ");
  for(int i = 0; i < sensorAmount; i++) {
    printSensor(sensors[i]);
    Serial.print("\n");
  }
}

void uploadMeasurement (HttpClient& client, Sensor& sensor) {
  Serial.print("Upload measurement for sensor: ");
  Serial.println(sensor.id);

  StaticJsonDocument<256> body;
  body["sensor_id"] = sensor.id;
  body["moisture"] = analogRead(sensor.pin);

  String bodyText;
  serializeJson(body, bodyText);

  Serial.print("Body: ");
  Serial.println(bodyText);

  client.beginRequest();
  client.post("/rest/v1/moisture");
  sendAuthorization(client);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", bodyText.length());
  client.sendHeader("Prefer", "return=minimal");
  client.beginBody();
  client.print(bodyText);
  client.endRequest();

  Serial.print("POST status code: ");
  Serial.println(client.responseStatusCode());
  Serial.print("POST response: ");
  Serial.println(client.responseBody());
}

void uploadMeasurements (HttpClient& client, int time) {
  for(int i = 0; i < sensorAmount; i++) {
    if (time % sensors[i].interval == 0) {
      uploadMeasurement(client, sensors[i]);
    }
  }
}

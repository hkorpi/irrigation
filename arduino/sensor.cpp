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

boolean initSensors(HttpClient& client) {
  String url = "/rest/v1/sensor?select=id,interval,pin";
  StaticJsonDocument<500> doc;
  boolean success = is200Ok(getJson(client, url, doc));

  if (success) {
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
  return success;
}

void uploadMeasurement (HttpClient& client, Sensor& sensor) {
  Serial.print("Upload measurement for sensor: ");
  Serial.println(sensor.id);

  StaticJsonDocument<256> body;
  body["sensor_id"] = sensor.id;
  body["value"] = analogRead(sensor.pin);

  String url = "/rest/v1/measurement";
  postJson(client, url, body);
}

void uploadMeasurements (HttpClient& client, int time) {
  for(int i = 0; i < sensorAmount; i++) {
    if (time % sensors[i].interval == 0) {
      uploadMeasurement(client, sensors[i]);
    }
  }
}

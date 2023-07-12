#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>

#include "pump.h"
#include "api-secrets.h"
#include "rest-api.h"

struct Pump {
  int id;
  int pin;
};

static Pump* pumps;
static int pumpAmount = 0;

void printPump (Pump& pump) {
  Serial.print(pump.id);
  Serial.print(", ");
  Serial.print(pump.pin);
}

boolean initPumps(HttpClient& client) {
  String url = "/rest/v1/pump?select=*";
  StaticJsonDocument<500> doc;
  boolean success = is200Ok(getJson(client, url, doc));

  if (success) {
    JsonArray json_pumps = doc.as<JsonArray>();
    pumpAmount = json_pumps.size();
    pumps = new Pump[pumpAmount];

    for(int i = 0; i < pumpAmount; i++) {
      pumps[i].id = (int) json_pumps[i]["id"];
      pumps[i].pin = (int) json_pumps[i]["pin"];
    }

    Serial.println("Found pumps: ");
    for(int i = 0; i < pumpAmount; i++) {
      printPump(pumps[i]);
      Serial.print("\n");
    }
  }

  return success;
}

void irrigate (HttpClient& client, Pump& pump) {
  Serial.print("Upload measurement for pump: ");
  Serial.println(pump.id);

  StaticJsonDocument<256> body;
  body["pump_id"] = pump.id;

  String bodyText;
  serializeJson(body, bodyText);

  Serial.print("Body: ");
  Serial.println(bodyText);

  client.beginRequest();
  client.post("/rest/v1/rpc/next_irrigation_job");
  sendAuthorization(client);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", bodyText.length());
  client.beginBody();
  client.print(bodyText);
  client.endRequest();

  Serial.print("POST status code: ");
  Serial.println(client.responseStatusCode());
  Serial.print("POST response: ");
  Serial.println(client.responseBody());
}

void irrigate (HttpClient& client, int time) {
  for(int i = 0; i < pumpAmount; i++) {
    irrigate(client, pumps[i]);
  }
}

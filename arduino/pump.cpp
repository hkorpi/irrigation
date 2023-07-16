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
  String url = "/rest/v1/pump?select=id,pin";
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
      pinMode(pumps[i].pin, OUTPUT);
    }
  }

  return success;
}

void irrigate (HttpClient& client, Pump& pump) {
  StaticJsonDocument<256> request;
  request["pump_id"] = pump.id;

  StaticJsonDocument<256> response;

  String url = "/rest/v1/rpc/next_irrigation_job";
  boolean success = is200Ok(postJsonAndJsonResponse(client, url, request, response));

  JsonObject nextJob = response.as<JsonObject>();
  if (success && !nextJob.isNull()) {
    int time = nextJob["irrigation_time"];
    request["valid_from"] = nextJob["valid_from"];
    Serial.print("A new irrigation job for pump: ");
    Serial.println(pump.id);

    String startUrl = "/rest/v1/rpc/start_irrigation_job";
    int startResponseStatus = postJson(client, startUrl, request);

    if (startResponseStatus == 204) {
      Serial.print("Pumping ");
      Serial.print(time);
      Serial.println(" s");
      digitalWrite(pump.pin, HIGH);
      delay(time*1000);
      digitalWrite(pump.pin, LOW);
      String endUrl = "/rest/v1/rpc/end_irrigation_job";
      postJson(client, endUrl, request, 3, 204, 5000);
    }
  } else {
    Serial.print("No irrigation jobs for pump: ");
    Serial.println(pump.id);
  }
}

void irrigate (HttpClient& client) {
  for(int i = 0; i < pumpAmount; i++) {
    irrigate(client, pumps[i]);
  }
}

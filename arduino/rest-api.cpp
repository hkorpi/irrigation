#include <ArduinoHttpClient.h>

#include "rest-api.h"
#include "api-secrets.h"

void sendAuthorization (HttpClient& client) {
  client.sendHeader("apikey", API_KEY);
  client.sendHeader("Authorization", API_AUTHORIZATION);
}

JsonResponseStatus getJson (HttpClient& client, String& url, JsonDocument& response) {
  Serial.print("GET: ");
  Serial.println(API_HOST + url);

  client.beginRequest();
  client.get("/rest/v1/sensor?select=*");
  sendAuthorization(client);
  client.endRequest();

  int responseStatus = client.responseStatusCode();
  Serial.print("Response status: ");
  Serial.println(responseStatus);

  client.skipResponseHeaders();

  return {responseStatus, deserializeJson(response, client)};
}

boolean is200Ok (JsonResponseStatus status) {
  return status.response == 200 && !status.json;
}
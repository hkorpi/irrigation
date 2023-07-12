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
  client.get(url);
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

int postJson (HttpClient& client, String& url, JsonDocument& body) {
  Serial.print("POST: ");
  Serial.println(API_HOST + url);

  client.beginRequest();
  client.post(url);
  sendAuthorization(client);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", measureJson(body));
  client.sendHeader("Prefer", "return=minimal");
  client.beginBody();
  serializeJson(body, client);
  client.endRequest();

  int responseStatus = client.responseStatusCode();
  Serial.print("Response status: ");
  Serial.println(responseStatus);

  return responseStatus;
}

JsonResponseStatus postJsonAndJsonResponse (
  HttpClient& client, String& url, 
  JsonDocument& request, JsonDocument& response) {
  
  Serial.print("POST: ");
  Serial.println(API_HOST + url);

  client.beginRequest();
  client.post(url);
  sendAuthorization(client);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", measureJson(request));
  client.beginBody();
  serializeJson(request, client);
  client.endRequest();

  int responseStatus = client.responseStatusCode();
  Serial.print("Response status: ");
  Serial.println(responseStatus);

  client.skipResponseHeaders();

  return {responseStatus, deserializeJson(response, client)};
}

int postJson (HttpClient& client, String& url, JsonDocument& body, 
              int maxRetryAmount, int successStatus, int sleep) {
  
  int counter = 0;
  while (postJson (client, url, body) != successStatus && 
         counter < maxRetryAmount) {
    counter++;
    delay(sleep);
  }
}
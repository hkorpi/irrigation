#include <ArduinoHttpClient.h>

#include "rest-api.h"
#include "api-secrets.h"

void sendAuthorization (HttpClient& client) {
  client.sendHeader("apikey", API_KEY);
  client.sendHeader("Authorization", API_AUTHORIZATION);
}
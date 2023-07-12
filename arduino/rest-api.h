#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

static const int HTTP_ERROR_INVALID_JSON =-5;

struct JsonResponseStatus {
  int response;
  DeserializationError json;
};

boolean is200Ok (JsonResponseStatus status);

void sendAuthorization (HttpClient& client);

JsonResponseStatus getJson (HttpClient& client, String& url, JsonDocument& response);

int postJson (HttpClient& client, String& url, JsonDocument& body);

int postJson (HttpClient& client, String& url, JsonDocument& body, 
              int maxRetryAmount, int successStatus, int sleep);

JsonResponseStatus postJsonAndJsonResponse (
  HttpClient& client, String& url, 
  JsonDocument& request, JsonDocument& response);
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
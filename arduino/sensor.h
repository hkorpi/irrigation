#include <ArduinoHttpClient.h>

boolean initSensors(HttpClient& client);

void uploadMeasurements (HttpClient& client, int time);
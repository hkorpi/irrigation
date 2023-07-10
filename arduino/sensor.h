#include <ArduinoHttpClient.h>

void initSensors(HttpClient& client);

void uploadMeasurements (HttpClient& client, int time);
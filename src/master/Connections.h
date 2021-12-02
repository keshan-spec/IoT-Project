#include <Arduino.h>

void connectToWiFi();
void writeToThingSpeak(struct ThingSpeakData data);
void strip_data(String rawdata);

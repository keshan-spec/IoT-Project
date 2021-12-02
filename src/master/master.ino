#include <SPI.h>
#include <LoRa.h>
#include <WiFiNINA.h>

#include "SECRETS.h"
#include <ThingSpeak.h>
#include "Connections.h"

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;        // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the WiFi radio's status
WiFiClient  client;

int bpm, humidity, temp_c, incomingLen;
String incoming;
float timer, ms, milliSeconds;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // connect to wifi
  connectToWiFi();
  // end wifi code

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  ThingSpeak.begin(client);  //Initialize ThingSpeak
  Serial.println("LoRa Status: [MASTER NODE RUNNING]");
}

unsigned long calculateDeltaTime(){
  
}

void loop() {
  ms = micros();
  milliSeconds = ms / 1000;
  timer += milliSeconds;
  Serial.println("Timer : " + String(timer) + "ms");

  if (timer > 16000){
    timer = 0;
    Serial.println("TIMER HIT! 15s");
  }

  int packetSize = LoRa.parsePacket(); // try to parse packet
  if (packetSize == 0) return;
  Serial.println("Received packet with RSSI " + String(LoRa.packetRssi()));

  incomingLen = LoRa.read();
  bpm = LoRa.read();
  humidity = LoRa.read();
  temp_c = LoRa.read();

  //  incoming = String(bpm) + String(humidity) + String(temp_c);
  //    if (incomingLen != incoming.length()) {
  //      Serial.println("Error, packets not received fully!");
  //      return;
  //    }

  Serial.print("BPM " + String(bpm));
  Serial.print("\tHumidity " + String(humidity));
  Serial.println("\tTemperature " + String(temp_c));

  if (timer >= 20000) {
    // upload to thingspeak
    ThingSpeak.setField(1, bpm);
    ThingSpeak.setField(4, humidity);
    ThingSpeak.setField(2, temp_c);

    int x = ThingSpeak.writeFields(CHANNEL_NO, WRITE_API_KEY);
    // reset timer
    timer = 0;

    if (x == 200) {
      Serial.println("Channel update successful");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }

  // total time that took for loop - start time of loop
  ms = micros() - ms;
  milliSeconds = ms;

  delay(20);
} // end of loop

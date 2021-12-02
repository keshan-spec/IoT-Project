#include <SPI.h>
#include <LoRa.h>
#include <WiFiNINA.h>

#include "SECRETS.h"
#include <ThingSpeak.h>
#include "Connections.h"

char ssid[] = SECRET_SSID;       // your network SSID (name)
char pass[] = SECRET_PASS;       // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the WiFi radio's status
WiFiClient  client;

int bpm, humidity, incomingLen;
float temp_c;
String incoming;
unsigned long WRITE_INTERVAL = 5 * 1000; // seconds in ms
unsigned long ms, timer, delta;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  connectToWiFi(); // connect to wifi

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  ThingSpeak.begin(client);  //Initialize ThingSpeak
  Serial.println("LoRa Status: [MASTER NODE RUNNING]");
}

void loop() {
  ms = micros();

  // write to thingspeak after 15 seconds interval
  if (delta > WRITE_INTERVAL) {
    delta = 0; timer = 0; // reset timer
    digitalWrite(13, HIGH);
    Serial.println(String(WRITE_INTERVAL / 1000) + " seconds lapsed");

    // upload to thingspeak
    ThingSpeak.setField(1, bpm);
    ThingSpeak.setField(4, humidity);
    ThingSpeak.setField(2, temp_c);

    int x = ThingSpeak.writeFields(CHANNEL_NO, WRITE_API_KEY);
    if (x != 200) Serial.println("Problem updating channel. HTTP error code " + String(x));
    // finish uploading

    delay(1000);
    digitalWrite(13, LOW);
  }

  // Reference: https://hutscape.com/tutorials/lora-duplex-a
  int packetSize = LoRa.parsePacket(); // try to parse packet
  if (packetSize == 0) return;
  Serial.println("Received packet with RSSI " + String(LoRa.packetRssi()));

  incomingLen = LoRa.read();
  bpm = LoRa.read();
  humidity = LoRa.read();
  temp_c = LoRa.read();

  incoming = String(bpm) + String(humidity) + String(temp_c);
  if (incoming.length() != incomingLen) {
    Serial.println("Error, packets not received fully!");
    return;
  }

  Serial.print("BPM " + String(bpm));
  Serial.print("\tHumidity " + String(humidity));
  Serial.println("\tTemperature " + String(temp_c));

  // total time that took for loop - start time of loop
  timer = (micros() - ms) / 1000;
  delta += timer;
  delay(20);
}

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
unsigned long WRITE_INTERVAL = 16 * 1000; // seconds in ms
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

void alert() {
  tone(8, 2000); // buzzer
  digitalWrite(11, HIGH);
  delay(300);

  noTone(8);
  digitalWrite(11, LOW);
  delay(300);

  tone(8, 2000); // buzzer
  digitalWrite(11, HIGH);
  delay(300);

  noTone(8);
  digitalWrite(11, LOW);
}

void loop() {
  ms = micros();

  // write to thingspeak after WRITE_INTERVAL
  if (delta > WRITE_INTERVAL) {
    delta = 0; timer = 0; // reset timer
    digitalWrite(13, HIGH);
    Serial.println(String(WRITE_INTERVAL / 1000) + " seconds lapsed");

    // upload to thingspeak
    ThingSpeak.setField(1, bpm);
    ThingSpeak.setField(2, temp_c); // celsuis
    //    ThingSpeak.setField(3, temp_c * 1.8 + 32); // fahrenhite
    ThingSpeak.setField(4, humidity);

    int x = ThingSpeak.writeFields(CHANNEL_NO, WRITE_API_KEY);
    if (x == 200) Serial.println("Channel Updated");
    else Serial.println("Problem updating channel. HTTP error code " + String(x));
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

  if (bpm >= 200) {
    alert();
  }
  //  Serial.print("BPM " + String(bpm));
  //  Serial.print("\tHumidity " + String(humidity));
  //  Serial.println("%\tTemperature " + String(temp_c));


  // total time that took for loop - start time of loop
  delay(20);
  timer = (micros() - ms) / 1000;
  delta += timer;
}

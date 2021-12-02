#include <SPI.h>
#include <LoRa.h>
#include <WiFiNINA.h>

#include "SECRETS.h"
#include <ThingSpeak.h>
#include "Connections.h"

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;        // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the WiFi radio's status

struct ThingSpeakData {
  unsigned long channelNum;
  String input;
  char * writeAPIKey;
};

String LoRaData; // string variable to store receiving packets

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("LoRa Status: [MASTER NODE RUNNING]");
  //  Serial.println("bpm,temp_c,temp_f,humidity");
}

void loop() {
  int packetSize = LoRa.parsePacket(); // try to parse packet

  if (packetSize) { // if packetsize is not empty then you have received a packet
    Serial.print("Received packet '");
    while (LoRa.available()) {   // read packet
      LoRaData += LoRa.readString(); // append recieving packets to one variable
    }
    Serial.print(LoRaData); // print collected data
    
    struct ThingSpeakData tsd = {
      .channelNum = CHANNNEL_NO,
      .input = LoRaData,
      .writeAPIKey = WRITE_API_KEY
    };
    
    writeToThingSpeak(tsd);
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  } // end of if
} // end of loop

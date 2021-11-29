//#include <HashMap.h>

#include "DFRobot_Heartrate.h"
#include "DHT.h"
#include <SPI.h>
#include <WiFiNINA.h>
#include <LoRa.h>

// custom header file
#include "SECRETS.h"
#include "SensorData.h"
#include "Connections.h"

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor for normal 16mhz Arduino
// Initialize Heart rate sensor for digital mode
DFRobot_Heartrate heartrate(DIGITAL_MODE); ///< ANALOG_MODE or DIGITAL_MODE
// Create a wifi client for ThingSpeak connection
WiFiClient  client;

int counter = 0;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //  hashMap[0]("HEART_SENSOR", 1);
  //  hashMap[1]("DHT11", 2);
  //  hashMap[2]("LIGHT_SENSOR", 3);
  //  hashMap[3]("MICROPHONE_SENSOR", 4);
  //
  //  Serial.println(hashMap.getIndexOf("HEART_SENSOR"), DEC );
  //  Serial.println(hashMap.getValueOf("HEART_SENSOR"));

  //  hashMap.debug();

  startSender(); // Begin LoRa Broadcasting
  dht.begin(); // Initialize dht
  //  ThingSpeak.begin(client);  //Initialize ThingSpeak
}

void loop() {
  delay(10000); // wait for 200ms to avoid errors

  // Display heart rate
  //  getHeartRate();

  // display temperature
  //  struct DHTData dhtdata = getDHTSensorData();
  //  if (!isnan(dhtdata.temp_c)) {
  //    Serial.println("Temp C: " + String(dhtdata.temp_c));
  //  }

  // start broadcasting
  // https://lora-developers.semtech.com/uploads/documents/files/Building_a_LoRa-based_Device_End-to-End_with_Arduino_Dnld_Final.pdf

  sendPacket(counter, 1);
  sendPacket(counter + 15, 2);
  counter ++;
}

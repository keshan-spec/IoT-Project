#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>
#include "dht.h"
#include <SPI.h>
#include <LoRa.h>

// header files
#include "DEFINITIONS.h"
#include "HeartPulseSensor.h"
#include "SensorData.h"
#include "LoRaHelper.h"

dht DHT; // Initialize DHT sensor for normal 16mhz Arduino
PulseSensorPlayground pulseSensor;
int bpm = 0;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  // wait for serial port to connect. Needed for native USB port only
  while (!Serial);

  initPulseSensor(); // Initialize the pulse sensor module
  startSender(); // Begin LoRa Broadcasting
  //  Serial.println("bpm,temp_c,temp_f,humidity");
}

void loop() {
  // Display heart rate
  if (pulseSensor.sawNewSample()) {
    if (--samplesUntilReport == (byte) 0) {
      samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
      if (pulseSensor.sawStartOfBeat()) {
        bpm = pulseSensor.getBeatsPerMinute();
        Serial.println("BPM : " + String(bpm));

        // display temperature
        // Wait a few seconds between measurements.
        delay(20);
        struct DHTData dhtdata = getDHTSensorData();
        if (!isnan(dhtdata.temp_c)) {
          Serial.println("Humidity: " + String(dhtdata.humidity) + "%\tTemp C: " + String(dhtdata.temp_c) + "\tTemp F: " + String(dhtdata.temp_f));
        }
        //        Serial.println(String(bpm)+","+String(dhtdata.temp_c)+","+String(dhtdata.temp_f)+","+String(dhtdata.humidity)+"");
        // broadcast bpm and dht data to LoRa node
        sendPacket(bpm, dhtdata);
      }
    }
  }

}

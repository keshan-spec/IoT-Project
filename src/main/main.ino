/*
  Every Sketch that uses the PulseSensor Playground must
  define USE_ARDUINO_INTERRUPTS before including PulseSensorPlayground.h.
  Here, #define USE_ARDUINO_INTERRUPTS false tells the library to
  not use interrupts to read data from the PulseSensor.
*/
#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>
#include "dht.h"
#include <SPI.h>
#include <LoRa.h>

// header files
#include "DEFINITIONS.h"

dht DHT; // Initialize DHT sensor for normal 16mhz Arduino
PulseSensorPlayground pulseSensor; // Initialize the pulse sensor module

void sendPacket(int bpm, int humidity, float temp_c) {
  Serial.print("Sending packet: ");
  Serial.print("BPM : " + String(bpm) + " | Temp C: ");
  Serial.print(String(temp_c) + ", Humidity: ");
  Serial.println(String(humidity) + "%");

  // send packet
  LoRa.beginPacket();
  /*
    Safety measure to make sure all writes are received,
    On the receiving node, a lenght check will determine if
    every byte is received.
  */
  String outgoing = String(bpm) + String(humidity) + String(temp_c);
  LoRa.write(outgoing.length());
  LoRa.write(bpm);
  LoRa.write(humidity);
  LoRa.write(temp_c);
  LoRa.endPacket();
}

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  // wait for serial port to connect. Needed for native USB port only
  while (!Serial);

  // Configure the PulseSensor manager.
  pulseSensor.analogInput(PULSE_INPUT);
  pulseSensor.blinkOnPulse(PULSE_BLINK);
  pulseSensor.setThreshold(THRESHOLD);

  // Skip the first SAMPLES_PER_SERIAL_SAMPLE in the loop().
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;

  // Now that everything is ready, start reading the PulseSensor signal.
  if (!pulseSensor.begin()) {
    Serial.println("Unable to start pulse sensor");
  }

  // Begin LoRa Broadcasting
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("Sender/Slave Node Status: [ACTIVE]\n\n");
}

void loop() {
  /*NOTE: if US_PS_INTERRUPTS is false, your Sketch must
    call pulse.sawNewSample() at least once every 2 milliseconds
    to accurately read the PulseSensor signal.
  */
  if (pulseSensor.sawNewSample()) {
    if (--samplesUntilReport == (byte) 0) {
      samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
      if (pulseSensor.sawStartOfBeat()) {
        bpm = pulseSensor.getBeatsPerMinute();

        // display temperature
        // Wait a few seconds between measurements.
        delay(20);
        DHT.read11(DHTPIN);

        // Reading temperature or humidity takes about 250 milliseconds!
        humidity = DHT.humidity;
        // Read temperature as Celsius
        temp_c = DHT.temperature;

        // Check if any reads failed and exit early (to try again).
        if (isnan(humidity) || isnan(temp_c)) {
          Serial.println("Failed to read from DHT sensor!");
        }

        // broadcast bpm and dht data to LoRa node
        sendPacket(bpm, humidity, temp_c);
      }
    }
  }
}

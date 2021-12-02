#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>
#include "dht.h"
#include <SPI.h>
#include <LoRa.h>

// header files
#include "DEFINITIONS.h"
#include "LoRaHelper.h"

dht DHT; // Initialize DHT sensor for normal 16mhz Arduino
PulseSensorPlayground pulseSensor;

int bpm, humidity, temp_c;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  // wait for serial port to connect. Needed for native USB port only
  while (!Serial);

  // Initialize the pulse sensor module
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
  // Display heart rate
  if (pulseSensor.sawNewSample()) {
    if (--samplesUntilReport == (byte) 0) {
      samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
      if (pulseSensor.sawStartOfBeat()) {
        bpm = pulseSensor.getBeatsPerMinute();
        //Serial.print("BPM : " + String(bpm));

        // display temperature
        // Wait a few seconds between measurements.
        delay(20);
        DHT.read11(DHTPIN);

        // Reading temperature or humidity takes about 250 milliseconds!
        // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
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

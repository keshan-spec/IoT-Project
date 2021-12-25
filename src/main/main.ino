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

// header file
#include "DEFINITIONS.h"

dht DHT; // Initialize DHT sensor for normal 16mhz Arduino
PulseSensorPlayground pulseSensor; // Initialize the pulse sensor module

// calculates heat index from humidity and temperature
int calculateHeatIndex() {
  int hi = 0.5 * (temp_c + 61.0 + ((temp_c - 68.0) * 1.2) + (humidity * 0.094));

  if (hi > 79) {
    hi = -42.379 + 2.04901523 * temp_c + 10.14333127 * humidity +
         -0.22475541 * temp_c * humidity +
         -0.00683783 * pow(temp_c, 2) +
         -0.05481717 * pow(humidity, 2) +
         0.00122874 * pow(temp_c, 2) * humidity +
         0.00085282 * temp_c * pow(humidity, 2) +
         -0.00000199 * pow(temp_c, 2) * pow(humidity, 2);

    if ((humidity < 13) && (temp_c >= 80.0) && (temp_c <= 112.0)) {
      hi -= ((13.0 - humidity) * 0.25) * sqrt((17.0 - abs(temp_c - 95.0)) * 0.05882);
    }
    else if ((humidity > 85.0) && (temp_c >= 80.0) && (temp_c <= 87.0)) {
      hi += ((humidity - 85.0) * 0.1) * ((87.0 - temp_c) * 0.2);
    }
  }

  return hi;
}

// broadcasts sensor data
void sendPacket(int bpm, int humidity, float temp_c, int heat_index) {
  Serial.print("Sending packet: ");
  Serial.print("BPM : " + String(bpm) + " | Temp C: ");
  Serial.print(String(temp_c) + ", Humidity: ");
  Serial.print(String(humidity) + "%");
  Serial.print(", Heat index: ");
  Serial.println(String(heat_index) + "%");

  // send packet
  LoRa.beginPacket();
  /*
    Safety measure to make sure all writes are received,
    On the receiving node, a lenght check will determine if
    every byte is received.
  */
  String outgoing = String(bpm) + String(humidity) + String(temp_c) + String(heat_index);
  LoRa.write(outgoing.length());
  LoRa.write(bpm);
  LoRa.write(humidity);
  LoRa.write(temp_c);
  LoRa.write(heat_index);
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
  if (pulseSensor.sawNewSample()) { // get new sample from sensor
    if (--samplesUntilReport == (byte) 0) {
      samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
      if (pulseSensor.sawStartOfBeat()) { // if a beat is found
        bpm = pulseSensor.getBeatsPerMinute(); // store the BPM in the bpm variable

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

        int heat_index = calculateHeatIndex(); // get heat index
        
        // broadcast bpm and dht data to LoRa node
        sendPacket(bpm, humidity, temp_c, heat_index);
      }
    }
  }
}

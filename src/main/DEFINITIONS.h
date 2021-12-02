// DEFINITIONS FOR CONSTANTS
#define DHTTYPE DHT11   // DHT 11
#define DHTPIN A1          // Analog pin 0

// HEART SENSOR CONSTANTS
const int PULSE_INPUT = A0;
const int PULSE_BLINK = 13;    // Pin 13 is the on-board LED
const int THRESHOLD = 550;   // Adjust this number to avoid noise when idle

byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

struct DHTData {
  int humidity;
  float temp_c;
  float temp_f;
};

#include <SPI.h>
#include <LoRa.h>
#include <WiFiNINA.h>

#include "SECRETS.h"
#include <ThingSpeak.h>
#include "Connections.h"

char ssid[] = SECRET_SSID;       // network SSID (name)
char pass[] = SECRET_PASS;       // network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the WiFi radio's status
WiFiClient  client;

int bpm, humidity, incomingLen, heat_index;
float temp_c;
String incoming;

const int HR_THRESHOLD = 100;

// PINS
int BUZZER = 8;
int ALARM_LED = 7;


// THINGSPEAK UPLOAD VARIABLES
char server[] = "api.thingspeak.com"; // ThingSpeak Server
char jsonBuffer[500] = "["; // Initialize the jsonBuffer to hold data

/* Collect data once every second and post data to ThingSpeak
  channel once every 15 seconds */
unsigned long lastConnectionTime = 0; // Track the last connection time
unsigned long lastUpdateTime = 0; // Track the last update time
const unsigned long updateInterval = 1L * 1000L; // Update once every second
const unsigned long WRITE_INTERVAL = 15 * 1000; // 15 seconds in ms

void setup() {
  // start serial
  Serial.begin(115200);
  while (!Serial);

  // set pinmode for led pin
  pinMode(ALARM_LED, OUTPUT);

  connectToWiFi(); // connect to wifi

  // begin lora in 915E6 band
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("LoRa Status: [MASTER NODE RUNNING]");
}

void alert() {
  tone(BUZZER, 2000); // buzzer
  digitalWrite(ALARM_LED, HIGH); // red
  delay(300);

  noTone(BUZZER);
  digitalWrite(ALARM_LED, LOW); // OFF
  delay(300);

  tone(BUZZER, 2000); // buzzer
  digitalWrite(ALARM_LED, HIGH); // red
  delay(300);

  noTone(BUZZER);
  digitalWrite(ALARM_LED, LOW); // red
}

void updatesJson(char* jsonBuffer, int bpm, int humidity, int temp_c, int hi) {
  /* JSON format for updates paramter in the API
    This examples uses the relative timestamp as it uses the
    "delta_t". You can also provide the absolute timestamp using the
    "created_at" parameter
    instead of "delta_t".
    "[{\"delta_t\":0,\"field1\":-70},{\"delta_t\":3,\"field1\":-
    66}]"
  */

  // Format the jsonBuffer as noted above
  strcat(jsonBuffer, "{\"delta_t\":");
  unsigned long deltaT = (millis() - lastUpdateTime) / 1000;
  size_t lengthT = String(deltaT).length();
  char temp[4];
  String(deltaT).toCharArray(temp, lengthT + 1);
  strcat(jsonBuffer, temp);
  strcat(jsonBuffer, ",");

  // fields
  long rssi = WiFi.RSSI();
  lengthT = String(rssi).length();

  // bpm
  strcat(jsonBuffer, "\"field1\":");
  String(bpm).toCharArray(temp, lengthT + 1);
  strcat(jsonBuffer, temp);
  strcat(jsonBuffer, ",");

  // temperature
  strcat(jsonBuffer, "\"field2\":");
  String(temp_c).toCharArray(temp, lengthT + 1);
  strcat(jsonBuffer, temp);
  strcat(jsonBuffer, ",");

  // heat index
  strcat(jsonBuffer, "\"field3\":");
  String(hi).toCharArray(temp, lengthT + 1);
  strcat(jsonBuffer, temp);
  strcat(jsonBuffer, ",");

  // humidity
  strcat(jsonBuffer, "\"field4\":");
  String(humidity).toCharArray(temp, lengthT + 1);
  strcat(jsonBuffer, temp);


  // end of json
  strcat(jsonBuffer, "},");

  // Update thingspeak channel every 15 seconds
  if (millis() - lastConnectionTime >= WRITE_INTERVAL) {
    size_t len = strlen(jsonBuffer);
    jsonBuffer[len - 1] = ']';
    httpRequest(jsonBuffer);
  }

  lastUpdateTime = millis(); // Update the last update time
}

// Updates the ThingSpeakchannel with data
void httpRequest(char* jsonBuffer) {
  /* JSON format for data buffer in the API
    This examples uses the relative timestamp as it uses the
    "delta_t". You can also provide the absolute timestamp using the
    "created_at" parameter
    instead of "delta_t".
    "{\"write_api_key\":\"YOUR-CHANNELWRITEAPIKEY\",\"updates\":[{\"delta_t\":0,\"field1\":-
    60},{\"delta_t\":15,\"field1\":200},{\"delta_t\":15,\"field1\":-66}]
  */
  // Format the data buffer as noted above
  char data[500] = "{\"write_api_key\":\"06VDYCQVOVG3TD4E\",\"updates\":";

  strcat(data, jsonBuffer);
  strcat(data, "}");

  // Close any connection before sending a new request
  client.stop();


  String data_length = String(strlen(data) + 1); //Compute the data buffer length
  Serial.println(data);
  client.println("POST /channels/" + String(CHANNEL_NO) + "/bulk_update.json HTTP/1.1"); // Replace YOUR-CHANNEL-ID with your ThingSpeak channel ID

  // POST data to ThingSpeak
  if (client.connect(server, 80)) {
    client.println("Host: api.thingspeak.com");
    client.println("User-Agent: mw.doc.bulk-update (Arduino ESP8266)");
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("Content-Length: " + data_length);
    client.println();
    client.println(data);
  } else {
    Serial.println("Failure: Failed to connect to ThingSpeak");
    return;
  }

  delay(250); //Wait to receive the response
  client.parseFloat();

  String resp = String(client.parseInt());
  Serial.println("Response code:" + resp); // Print the response code. 202 indicates that the server has accepted the response
  jsonBuffer[0] = '['; //Reinitialize the jsonBuffer for next batch of data
  jsonBuffer[1] = '\0';
  lastConnectionTime = millis(); //Update the last conenction time
}


void loop() {
  // Reference: https://hutscape.com/tutorials/lora-duplex-a
  int packetSize = LoRa.parsePacket(); // try to parse packet
  if (packetSize == 0) return;
  Serial.println("Received packet with RSSI " + String(LoRa.packetRssi()));

  // read the packets in the order it was sent
  incomingLen = LoRa.read(); // cross validation packet
  bpm = LoRa.read();
  humidity = LoRa.read();
  temp_c = LoRa.read();
  heat_index = LoRa.read();

  // calculate the total recieved packets & compare if the sent lenght and received lenght matches
  incoming = String(bpm) + String(humidity) + String(temp_c) + String(heat_index);
  if (incoming.length() != incomingLen) {
    Serial.println("Error, packets not received fully!");
    return;
  }

  // if heartrate is higher than the threshold (200), raise an alarm
  if (bpm >= HR_THRESHOLD) alert();

  if (millis() - lastUpdateTime >= updateInterval) {
    updatesJson(jsonBuffer, bpm, humidity, temp_c, heat_index);
  }

  // small delay to avoid any loop errors
  delay(20);
}

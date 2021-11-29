char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;        // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the WiFi radio's status

void connectToWiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  Serial.println("SSID: " + String(WiFi.SSID()));
}

//void writeToThingSpeak(struct ThingSpeakData data) {
//  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
//  // pieces of information in a channel.
//  int x = ThingSpeak.writeField(data.channelNum, data.field, data.input, data.writeAPIKey);
//  if (x == 200) {
//    Serial.println("Channel update successful.");
//  }
//  else {
//    Serial.println("Problem updating channel. HTTP error code " + String(x));
//  }
//}

// LORA
void startSender() {
  Serial.println("LoRa Sender");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("Sender/Slave Node Status: [ACTIVE]\n\n");
}

void sendPacket(float data, int sensor) {
  Serial.print("Sending packet: ");
  Serial.println("Sensor : " + String(sensor) + "\tData : " + String(data));

  // send packet
  LoRa.beginPacket();
  LoRa.print(String(sensor)+ ", ");
  LoRa.print(data);
  LoRa.endPacket();
}

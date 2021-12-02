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

void writeToThingSpeak(struct ThingSpeakData data) {
  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.

  char str_array[data.input.length() + 1]; // create a char array in the size of the packet data
  data.input.toCharArray(str_array, data.input.length()); // convert string data to char array

  char *strings[4]; // an array of pointers to the pieces of the above array after strtok()
  char *ptr = NULL;
  byte index = 0;

  ptr = strtok(str_array, ",");  // delimiter = , (comma)
  while (ptr != NULL)  {
    strings[index] = ptr;
    index++;
    ptr = strtok(NULL, ",");
  }

  for (int i = 0; i < 4; i++) {
    // (CHANNEL NUMBER, FIELD, INPUT, WRITE KEY)
    int x = ThingSpeak.writeField(data.channelNum, atoi(strings[i]), atoi(strings[i]), data.writeAPIKey);

    if (x == 200) {
      Serial.println("Channel update successful [Field" + String(strings[i + 1]) + " ]");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }
}

void stripData(String rawdata) {
  char str_array[rawdata.length() + 1]; // create a char array in the size of the packet data
  rawdata.toCharArray(str_array, rawdata.length()); // convert string data to char array

  char *strings[4]; // an array of pointers to the pieces of the above array after strtok()
  char *ptr = NULL;
  byte index = 0;

  ptr = strtok(str_array, ",");  // delimiter = , (comma)
  while (ptr != NULL)  {
    strings[index] = ptr;
    index++;
    ptr = strtok(NULL, ",");
  }

  // use the atoi() and atof() functions to convert ASCII strings to numbers.
  // heart_bpm = atoi(strings[0]); //http://www.cplusplus.com/reference/cstdlib/atoi/?kw=atoi
  // temperature = atoi(strings[1]);
  //  return strings;
}

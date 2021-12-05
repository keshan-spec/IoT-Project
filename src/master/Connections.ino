void connectToWiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print(".");

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(2000);
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");
  Serial.println("SSID: " + String(WiFi.SSID()));
}

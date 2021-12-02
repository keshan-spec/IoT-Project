// LORA
// https://lora-developers.semtech.com/uploads/documents/files/Building_a_LoRa-based_Device_End-to-End_with_Arduino_Dnld_Final.pdf
void startSender() {
  Serial.println("LoRa Sender");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("Sender/Slave Node Status: [ACTIVE]\n\n");
}


void sendPacket(int bpm, DHTData dht) {
  Serial.print("Sending packet: ");
  Serial.print("BPM : " + String(bpm) + "\tDHT Data : ");
  Serial.print(String(dht.temp_c) + " C,");
  Serial.print(String(dht.temp_f) + " F,");
  Serial.println(String(dht.humidity) + "%");


  // send packet
  LoRa.beginPacket();
  LoRa.print(String(bpm) + "," + String(dht.temp_c) + "," + String(dht.temp_f) + "," + String(dht.humidity));
  LoRa.endPacket();
}

// LORA
// https://lora-developers.semtech.com/uploads/documents/files/Building_a_LoRa-based_Device_End-to-End_with_Arduino_Dnld_Final.pdf
void sendPacket(int bpm, float humidity, float temp_c) {
  Serial.print("Sending packet: ");
  Serial.print("BPM : " + String(bpm) + " | Temp C: ");
  Serial.print(String(temp_c) + ", Humidity: ");
  Serial.println(String(humidity) + "%");

  String outgoing = String(bpm) + String(humidity) + String(temp_c);

  // send packet
  LoRa.beginPacket();
  //  LoRa.print(String(bpm) + "," + String(temp_c) + "," + String(humidity));
  LoRa.write(outgoing.length());
  LoRa.write(bpm);
  LoRa.write(humidity);
  LoRa.write(temp_c);
  LoRa.endPacket();
}

// https://randomnerdtutorials.com/complete-guide-for-dht11dht22-humidity-and-temperature-sensor-with-arduino/

struct DHTData getDHTSensorData() {
  // Wait a few seconds between measurements.
  delay(1000);

  struct DHTData data;

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  data.humidity = dht.readHumidity();
  // Read temperature as Celsius
  data.temp_c = dht.readTemperature();
  // Read temperature as Fahrenheit
  data.temp_f = dht.readTemperature(true);



  // Check if any reads failed and exit early (to try again).
  if (isnan(data.humidity) || isnan(data.temp_c) || isnan(data.temp_f)) {
    Serial.println("Failed to read from DHT sensor!");
    return data;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  data.heat_index = dht.computeHeatIndex(data.temp_f, data.humidity);

  return data;
}

void getHeartRate() {
  uint8_t rateValue;
  heartrate.getValue(DFROBOT_HR_PIN); ///< A1 foot sampled values
  rateValue = heartrate.getRate(); ///< Get heart rate value
  if (rateValue)  {
    Serial.println(rateValue);
  }
  delay(20);
}

void getSoundData() {
  int sen = 0;
  // https://www.velleman.eu/downloads/29/vma309_a4v02.pdf
  sen = analogRead(A0);
  delay(20);
  Serial.println(sen, DEC);
}

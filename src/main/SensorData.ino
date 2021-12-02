//https://www.brainy-bits.com/post/how-to-use-the-dht11-temperature-and-humidity-sensor-with-an-arduino
struct DHTData getDHTSensorData() {
  struct DHTData data;
  DHT.read11(DHTPIN);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  data.humidity = DHT.humidity;
  // Read temperature as Celsius
  data.temp_c = DHT.temperature;

  // Check if any reads failed and exit early (to try again).
  if (isnan(data.humidity) || isnan(data.temp_c)) {
    Serial.println("Failed to read from DHT sensor!");
  }

  return data;
}

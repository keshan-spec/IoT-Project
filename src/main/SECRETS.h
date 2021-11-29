// DEFINITIONS FOR CONSTANTS
#define DHTTYPE DHT11   // DHT 11
struct DHTData {
  float humidity;
  float temp_c;
  float temp_f;
  int heat_index;
};

struct ThingSpeakData {
  unsigned long channelNum;
  int field;
  float input;
  char * writeAPIKey;
};

//const byte HASH_SIZE = 5;
////storage
//HashType<char*, int> hashRawArray[HASH_SIZE];
////handles the storage [search,retrieve,insert]
//HashMap<char*, int> hashMap = HashMap<char*, int>( \hashRawArray , HASH_SIZE );


// SENSOR PINS
#define DFROBOT_HR_PIN A1 // Analog pin 0
#define DHTPIN 7          // Digital pin 7
#define MICROPHONE_PIN A5 // Analog pin 5

// WiFi Details
#define SECRET_SSID "VM21E0C8"
#define SECRET_PASS "vhrun7eamsjQ"


// API Keys

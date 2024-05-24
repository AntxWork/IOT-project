#define BLYNK_TEMPLATE_ID "TMPL6xJOzUSWP"
#define BLYNK_TEMPLATE_NAME "Ant"
#define BLYNK_AUTH_TOKEN "5PoYbktSZQ_JtbWMTdzsmq9gyeHTD16I"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial
#include <espnow.h>
#include <DHT.h>
#define SSID "Aaaa"     // WiFi network name
#define PASSWORD "11111111"  // WiFi password
#define DHTPIN D1       // Pin connected to the DHT sensor
#define DHTTYPE DHT11   // Type of DHT sensor

char auth [] = BLYNK_AUTH_TOKEN;
// Replace with the MAC Address of your NodeMCU#1
uint8_t broadcastAddress[] = {0x08, 0xF9, 0xE0, 0x70, 0xF7, 0x9E};

float humidity;
float temperature;

// Structure to hold DHT sensor readings
typedef struct {
  float temp;
  float humdt;
} SensorData;

SensorData sensorData;

DHT dht(DHTPIN, DHTTYPE);

// Callback function when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, SSID ,PASSWORD);
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n", SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    dht.begin();
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : %s\n", WiFi.localIP().toString().c_str());

  // Initialize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Add NodeMCU1 as a peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_CONTROLLER, 1, NULL, 0);

  esp_now_register_send_cb(OnDataSent);

  // Initialize DHT sensor
  dht.begin();
}

void loop() {
  delay(2000);

  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // Check if any reads failed and exit early
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Store sensor readings in the struct
  sensorData.temp = temperature;
  sensorData.humdt = humidity;

  // Send data via ESP-NOW
  esp_now_send(broadcastAddress, (uint8_t*)&sensorData, sizeof(sensorData));
  Blynk.virtualWrite(V2,temperature);
  Blynk.virtualWrite(V3,humidity);

} //ตัวส่งอุณหภูมิไปยังnodemcu&blynkได้ทั้งคู่
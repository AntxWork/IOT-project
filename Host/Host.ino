#define BLYNK_TEMPLATE_ID "TMPL6xJOzUSWP"
#define BLYNK_TEMPLATE_NAME "Ant"
#define BLYNK_AUTH_TOKEN "5PoYbktSZQ_JtbWMTdzsmq9gyeHTD16I"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial
#include <espnow.h>
#define SSID "Aaaa"     // WiFi network name
#define PASSWORD "11111111"  // WiFi password
#define RELAY_PIN D2 // Change this to the appropriate pin number
char auth [] = BLYNK_AUTH_TOKEN;
int check = 0;
BlynkTimer timer;
// REPLACE WITH NodeMCU#2 MAC Address
uint8_t broadcastAddress[] = {0x10, 0x52, 0x1C, 0xE5, 0xAF, 0xD5};

// Structure to declare variable to copy Dht values
typedef struct dhtStruct {
  float temp;
  float humdt;
} dhtStruct;

dhtStruct dhtObj; //Create a dhtStruct objwct called dhtObj

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&dhtObj, incomingData, sizeof(dhtObj));
  Serial.print("Temperature : ");
  Serial.println(dhtObj.temp);
  Serial.print("Humidity : ");
  Serial.println(dhtObj.humdt);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(V0, OUTPUT);
  Blynk.begin(auth, SSID ,PASSWORD);
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n", SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : %s\n", WiFi.localIP().toString().c_str());
  Serial.begin(115200); // Init Serial Monitor
  
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER); //set role
  esp_now_register_send_cb(OnDataSent); //set callback for data sent
  esp_now_register_recv_cb(OnDataRecv);  //set callback for data received
 
  // add NodeMCU 2 as a peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
}
BLYNK_WRITE(V0){
  Serial.print("v0");
  int value = param.asInt();
  if(value==1){
    check = 0;
   digitalWrite(RELAY_PIN,LOW); 
    Serial.print("vin");
  }
  else{
    check = 1;
    digitalWrite(RELAY_PIN,HIGH); 
    delay(1000);
    Serial.print("vin2");
  }
}
void loop() {
  Blynk.run();
  timer.run();
  if(check = 0){
 if(dhtObj.humdt < 53.0){
    digitalWrite(RELAY_PIN,LOW );
  }else if (dhtObj.humdt > 60.0){
    digitalWrite(RELAY_PIN, HIGH);
  }

  }
}
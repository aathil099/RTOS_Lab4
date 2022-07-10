#include <ThingSpeak.h>
#include <WiFi.h>

#define RELAY_PIN 5


const char* ssid = "En20416362";
const char* password = "wifi12345";

// ThingSpeak information
char* server = "api.thingspeak.com";
unsigned long channelID = 1709287;
char* readAPIKey = "9JIL6QCWWX7VVF1Y";
unsigned int dataFieldOne = 1;  

// This constant is device specific..
float Const_val = 2.25E-02;
WiFiClient client; //will be used by the thingspeak library to make the http request

float readTSData( long TSChannel, unsigned int TSField ) {
    float data =  ThingSpeak.readFloatField( TSChannel, TSField, readAPIKey );
    Serial.println( " Data read from ThingSpeak: " + String( data, 9 ) );
    return data;
}
void cloud_read( void * pvParameters ){
  
  for(;;){
    vTaskDelay(15000/portTICK_PERIOD_MS);
    Serial.println("Waiting...");
    //reading data from thing speak
  
  Const_val = readTSData( channelID, dataFieldOne );
  Serial.println(Const_val);
  
    if (Const_val < 50){
    digitalWrite(RELAY_PIN, LOW); // turn on pump 4 seconds
    Serial.println("Motor is ON");
    
    }else if(Const_val > 50){
    digitalWrite(RELAY_PIN, HIGH);  // turn off pump 4 seconds
    Serial.println("Motor is OFF");
    }
  }
}

QueueHandle_t xQueue;
   
void setup() {
  Serial.begin(9600);
  Serial.println("Start");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(500/portTICK_PERIOD_MS);
  } 
  ThingSpeak.begin(client);
  
  // Read the constants at startup.
  Const_val = readTSData(channelID, dataFieldOne);
  vTaskDelay(1500/portTICK_PERIOD_MS);


  xQueue = xQueueCreate (8, sizeof(int));
      if(xQueue !=NULL){
        xTaskCreatePinnedToCore(cloud_read, "cloudread", 1024,NULL,1,NULL,0);
        vTaskStartScheduler();
      }else{
        Serial.println("Queue has not been created");
      }
}

void loop() {

}

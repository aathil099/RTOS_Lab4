#include <ThingSpeak.h>
#include <DHT.h>
#include <WiFi.h>
#include <LiquidCrystal.h>
//#include <lcd.h>
#define dht_pin 13   
#define soilMoisture_pin 15         
#define dht_sensor_type DHT11
#define channel_ID 1794470
#define apiKey "YXN4C5COOTIA451Q"

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
 static const BaseType_t app_cpu = 0;
#else
 static const BaseType_t app_cpu = 1;
#endif


// Task handles
static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;


const char *ssid = "V2027" ;          //"EN20415686";      // wifi ssid key
const char *pass = "dias12345" ;      //"12345678";        // wifi ssid password


float humidity;
float temperature;
 
float soilMoistureValue;
int soilMoisturePercent;
const int AirValue = 2000;
const int WaterValue = 4200;

DHT dht_sensor(dht_pin, dht_sensor_type);

WiFiClient client;


// Task1: Upload the humidity and temperature sensor readings to Thingspeak server.

void Task1(void *parameter){ 
  Serial.println("Task1_______");
  while (1) {
    // read humidity
    humidity  = dht_sensor.readHumidity();
    // read temperature in Celsius
    temperature = dht_sensor.readTemperature();

    // check whether the reading is successful or not
    if ( isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      lcd.clear();
      lcd.print("Failed to read");
      lcd.setCursor(0, 1);
      lcd.print("from DHT sensor!");
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      Serial.print("Humidity: ");
      lcd.print("Hum:");
      lcd.print(humidity);
      lcd.print("% | ");
      Serial.print(humidity);
      Serial.print("%");

      Serial.print("  |  ");

      Serial.print("Temperature: ");
      lcd.print("Tem:");
      lcd.print(temperature);
      lcd.print("°C");
      Serial.print(temperature);
      Serial.println("°C");

      ThingSpeak.setField(1, temperature);
      ThingSpeak.setField(2, humidity);
      ThingSpeak.writeFields(channel_ID,apiKey);
      delay(5000);
   } 
  }
}

// Task2: Upload soil moisture sensor readings to Thingspeak server.

void Task2(void *parameter){
  Serial.println("Task2_______");
  while (1) {
    Serial.print("Soil Moisture: ");
    soilMoistureValue = analogRead(soilMoisture_pin);  //put Sensor insert into soil
    soilMoisturePercent = map(soilMoistureValue, WaterValue, AirValue, 0, 100);
    //soilMoisturePercent = ((soilMoistureValue-WaterValue)/(AirValue-WaterValue)) ;
    Serial.print(soilMoisturePercent);  
    Serial.println("%");
    lcd.setCursor(0, 1);
    lcd.print("Soil Mois:");
    lcd.print(soilMoisturePercent);
    lcd.print("%");

    ThingSpeak.setField(3, soilMoisturePercent);
    ThingSpeak.writeFields(channel_ID,apiKey);
    delay(5000); 
  }
}

void setup(){
  Serial.begin(115200);
  dht_sensor.begin();   //initialize the DHT sensor
  LiquidCrystal lcd(19, 23, 18, 17, 16, 4);  // initialize the library with the numbers of the interface pins
  lcd.begin(16, 2);     //set up the number of columns and rows:
  ThingSpeak.begin(client);
  
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    lcd.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  lcd.clear();
  lcd.print("WiFi connected");
  delay(4000);
 
 
  xTaskCreatePinnedToCore(
    Task1, //Function to implement the task
    "Task1", //Name of the task
    3000, //Stack size
    NULL, //Task input parameter 
    1, //Task priority
    &task_1, //Task handle
    0); //Task running core
    
  xTaskCreatePinnedToCore(
    Task2, //Function to implement the task
    "Task2", //Name of the task
    3000, //Stack size
    NULL, //Task input parameter 
    1, //Task priority
    &task_2, //Task handle
    1); //Task running core
}

void loop() {
 while(1){}
}

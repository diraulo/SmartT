
#include <Arduino.h>
#include <CS_MQ7.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561.h>
#include <DHT.h>

/*
 * Temperature and Humidity Preparation
 *************************************************/
#define DHTPIN A0 ///---first Sensor 
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

/*
 * Light Sensor
 *********************************/
Adafruit_TSL2561 tsl = Adafruit_TSL2561(TSL2561_ADDR_FLOAT, 12345);

/*
 * Carbon Monoxide Sensor
 *****************************/
CS_MQ7 MQ7(12, 13);

int temp_cumul = 0;

/*
 * Arduino Setup function (called automatically at startup)
 **********************************************************************************/
void setup() {
  Serial.begin(9600);
  Serial.println("Multi-Sensor Sensor Activate.");
  dht.begin();  
  if(!tsl.begin())
  {
    Serial.print("Ooops, no TSL2561 detected... Check your wiring or I2C ADDR!");
    while(1);
  }
  displaySensorDetails();
  ConfigureSensor();
  Serial.println("");
  delay(6000);
}

void loop() {   
  //Initialize Temperature and Humidity Sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  //Initialize Carbon Monoxide Sensor
  int CoSensorOutput = A1; 
  int CoData = 0;         
  MQ7.CoPwrCycler();    
 
  //Initialize Light Sensor
  sensors_event_t event;
  tsl.getEvent(&event);
  
  //Reading Data 
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT"); 
  } else {
    //Serial.print("Humidity: ");
    Serial.print(h);
    //Serial.print(" %\t");
    //Serial.print("                Temperature: ");
    Serial.print(t);
    //Serial.println(" *C");
    temp_cumul += (int)(t);
  }
    
  //Serial.print("C0 Concertration: ");
  
  if(MQ7.CurrentState() == LOW){ 
    CoData = analogRead(CoSensorOutput);
    Serial.print(CoData);
  } else{                            
    Serial.print(" Sensor heating!");
  }      
  
  
  //Serial.print("       Light Intensity: "); 
  if (event.light){
   Serial.print(event.light); 
   Serial.println(" lux");
  }
  
  
  delay(15000);  
}

/*
 * Configure Light Sensor
 ********************************/
void ConfigureSensor(){
  tsl.enableAutoGain(true); 
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
  Serial.println("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");
}

void sendData() {
  int averageTemp = temp_cumul / 4;
  
  //To-Do write the instruction to send data to server
}

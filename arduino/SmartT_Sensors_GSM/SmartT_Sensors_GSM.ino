/**
 * This code integrate both sensor data reading
 * and GSM Communication
 *
 * At the moment the integration is keeping GSM shield
 * from connecting and sending data to server. Due to time
 * constraint at the challenge, we couldn't fix the problem
 * Will work on it later
 */
 
#include <Arduino.h>
/*#include <CS_MQ7.h>  //Uncomment this to integrate sensors libraries
#include <Wire.h>
#include "TSL2561.h"
#include <DHT.h>*/

#include "inetGSM.h"
#include <SoftwareSerial.h>

/* GSM */
//#define SMART_SERVER "cloud.house4hack.co.za"
//#define SMART_PORT 8888
//#define SMART_SERVER "smartt.iriscouch.com"
#define SMART_SERVER "corrie206.co.za"
#define SMART_PORT 80
//#define GSM_WRITE DEBUG.print
#define GSM_WRITE gsm.SimpleWrite
//#define GSM_WRITELN DEBUG.println
#define GSM_WRITELN gsm.SimpleWriteln 

const char* server = SMART_SERVER;
int port = SMART_PORT;

#define HOWOFTEN_READ     10000  //Read the sensors every 10s
#define HOWOFTEN_UPLOAD   50000   //Upload data evey 1min
#define DEBUG_ON
#define DEBUG_LOCAL
InetGSM inet;
char result[10];
int numdata;

int i=0;
boolean started=false;
long lastReadMillis = 0;
long lastUploadMillis = 0;
#define ALPHA 0.9

#define DEBUG Serial

#define WAIT_TIMEOUT 1000

#define RETRY_COUNT 10
#define READ_RETRY 3
#define CHANNEL_COUNT 6
#define READING_SIZE 6
#define CLEAR_VALUE 0

char idstart = '0';
int readings[READING_SIZE];  //Will Store the Sensors readings


/*
 * Temperature and Humidity Preparation
 *************************************************/
/*#define DHTPIN A0 ///---first Sensor 
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);*/

/*
 * Light Sensor
 *********************************/
//TSL2561 tsl(TSL2561_ADDR_FLOAT); 

/*
 * Carbon Monoxide Sensor
 *****************************/
//CS_MQ7 MQ7(12, 13);

int temp_cumul = 0;
int count = 0;

/*
 * Arduino Setup function (called automatically at startup)
 **********************************************************************************/
void setup() {
  //Initialialize GSM
  for(int i=0;i<CHANNEL_COUNT; i++) {
     readings[i] = 0;
  }   

  //Serial connection.
  DEBUG.begin(9600);
  DEBUG.println("GSM Master");
  GSMBoot();
  
  
  /* Uncomment to initialise sensor data
  Serial.begin(9600);
  Serial.println("Multi-Sensor Sensor Activate.");
  dht.begin();  
  if(!tsl.begin())
  {
    Serial.print("Ooops, no TSL2561 detected... Check your wiring or I2C ADDR!");
    while(1);
  }
  //TSL2561 Sensor configuration - Light Sensor
  ConfigureSensor();*/
}

void loop() {   
  
  if(millis() < lastReadMillis || millis() < lastUploadMillis){
      lastReadMillis = millis();       
      lastUploadMillis = millis();
  }
  if(millis() - lastReadMillis > HOWOFTEN_READ){
    lastReadMillis = millis();
    /*readSensors();  //Uncomment to read sensors
    if (count >= 3) {
      readings[0] = temp_cumul / 4;  //Assign the average temperature value after 4 reading
      count = 0;
      temp_cumul = 0;
    }*/
  }  
  if(millis() - lastUploadMillis > HOWOFTEN_UPLOAD){
   
    byte number_loaded = uploadData();
    lastUploadMillis = millis();    
  }  
}

/*
 * Configure Light Sensor
 ********************************/
void ConfigureSensor(){
 /* tsl.setGain(TSL2561_GAIN_16X);   //Uncomment this block
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS); 
  Serial.println("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");*/
}

void readSensors() {
 /* //Initialize Temperature and Humidity Sensor
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  
  //Initialize Carbon Monoxide Sensor
  int CoSensorOutput = A1; 
  int CoData = 0;         
  MQ7.CoPwrCycler();    
 
  //Initialize Light Sensor
  uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);  
  uint32_t lum;
  
  //Reading Data 
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT"); 
  } else {
    readings[1] = h;  //Assign the humidity
    
    temp_cumul += t;  //Accumulate the temperature values
    count++;
  }
    
  //Serial.print("C0 Concertration: ");
  
  if(MQ7.CurrentState() == LOW){ 
    CoData = analogRead(CoSensorOutput);
    readings[4] = CoData;
    Serial.print(CoData);
  } else{                            
    Serial.print(" Sensor heating!");
  }      
  
   readings[2] = tsl.getFullLuminosity();*/

}

/************************************ GSM *********************************/
byte doConnect(){ 
    byte result = 0;
    DEBUG.println("Connecting to ");
    DEBUG.println(server);

    int n_of_at=0;
    
    while(n_of_at<10){
	  if(!inet.connectTCP(server, port)){
	  	#ifdef DEBUG_ON
			DEBUG.println("DB:NOT CONN");
		#endif	
	    	n_of_at++;
	  }else{
		result = 1;
		break;
  	  }
    }


    return(result);    
}

byte GSMBoot(){
    DEBUG.println("Booting GSM");
    gsm.begin(2400);  // reset
    //gsm.forceON();
    char result = 0;
    if (inet.attachGPRS("internet", "", "")){
      DEBUG.println("status=ATTACHED");
      result = 1;
    } else {
        DEBUG.println("status=ERROR");
        result = 0;
    }  
    delay(100);
 
}

byte uploadData(){
      byte number_uploaded = 0;
      char buf[150];

      
      buf[0] = 0;
      sprintf(buf, "{\"type\":\"weather\",\"id\":\"W123\",\"temperature\":%d,\"humidity\":%d,\"light\":%d, \"power\":%d, \"cox\":%d,\"rain\":%d, \"lat\":-25.85, \"lon\":27.50}",
         readings[0], readings[1], readings[2], readings[3], readings[4], readings[5] );
      #ifdef DEBUG_GSM
         DEBUG.println(buf);
      #endif   
      byte doc = doConnect();
      if(doc == 0) {
         DEBUG.print("Could not connect to ");
         DEBUG.println(server);
         DEBUG.println("Restarting");
         GSMBoot();
         doc = doConnect();              
      }
      if(doc == 1){             
         byte n = smarttPUT(buf, result, 10);
         doDisconnect();

         //Print the results.
         DEBUG.println("\nNumber of data received:");
         DEBUG.println(n);  
         if(n >0) {
            number_uploaded += n;
         } 

      } else {
         DEBUG.println("Could not upload data");
      }        
    return(number_uploaded);  
}


int smarttPUT( char* parameters, char* result, int resultlength)
{
  int num_char;

  char end_c[2];
  end_c[0]=0x1a;
  end_c[1]='\0';

  GSM_WRITE("POST /couchdbroot/smartt/_design/smartt/_update/sensor/ HTTP/1.1\nHost: ");
  GSM_WRITE(server);
  GSM_WRITE(":");
  GSM_WRITE(port);  
  GSM_WRITE("\n");
 
  GSM_WRITE("Content-Length: ");
  //itoa(strlen(parameters),itoaBuffer,10);
  GSM_WRITE(strlen(parameters));   
 
  GSM_WRITE("\nContent-Type: application/json");
  GSM_WRITE("\n\n");

  //char buf[40];
  //parameters.toCharArray(buf, 40);
  GSM_WRITE(parameters);
  GSM_WRITE("\n");
  GSM_WRITE(end_c);
  delay(1000);

  switch(gsm.WaitResp(10000, 100, "SEND OK")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }
  delay(5000);
	#ifdef DEBUG_ON
		DEBUG.println("DB:SENT");
	#endif	
  int res= gsm.read(result, resultlength);

  
  return res;
}

void doDisconnect(){
  inet.disconnectTCP();
}

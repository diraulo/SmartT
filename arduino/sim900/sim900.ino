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

#define HOWOFTEN_READ     1000
#define HOWOFTEN_UPLOAD   1000
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

/* RS 485 */
//#define DEBUG485_ON

#define DEBUG Serial


#define WAIT_TIMEOUT 1000

#define RETRY_COUNT 10
#define READ_RETRY 3
#define CHANNEL_COUNT 6
#define READING_SIZE 6
#define CLEAR_VALUE 0

char idstart = '0';
int readings[READING_SIZE];

void setup() 
{
 
  for(int i=0;i<CHANNEL_COUNT; i++) {
     readings[i] = 0;
  }   

  //Serial connection.
  DEBUG.begin(9600);
  DEBUG.println("GSM Master");
  GSMBoot();
};


void loop() 
{
  if(millis() < lastReadMillis || millis() < lastUploadMillis){
      lastReadMillis = millis();       
      lastUploadMillis = millis();
  }
  if(millis() - lastReadMillis > HOWOFTEN_READ){
    lastReadMillis = millis();
    //readSensors();
    readings[0] = analogRead(A0) / 45;
    readings[1] = analogRead(A0) / 10;
    readings[2] = analogRead(A0) / 250;
    readings[3] = analogRead(A0) / 50;
    readings[4] = analogRead(A0) / 153;
    readings[5] = analogRead(A0) / 70;
    
    for (int i = 0; i<6; i++) {
      DEBUG.println(readings[i]);
    }
  }  
  if(millis() - lastUploadMillis > HOWOFTEN_UPLOAD){
   
    byte number_loaded = uploadData();
    lastUploadMillis = millis();    
  }
   //   doConnect();
   // uploadData();

};




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
      sprintf(buf, "{\"type\":\"weather\",\"id\":\"W123\",\"temperature\":%d,\"humidity\":%d,\"light\":%d, \"power\":%d, \"cox\":%d,\"rain\":%d, \"lat\":-25.746, \"lon\":28.267}",
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

/************************************ RS 485 *********************************/


void readSensors(){
  //RS485.begin(9600);
  for(int i=0;i<CHANNEL_COUNT; i++) {
     int val = analogRead(A0+i); 
     readings[i] = val*(1-ALPHA) + readings[i]*ALPHA; 
  }   
}



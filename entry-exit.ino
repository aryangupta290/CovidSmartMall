#define BLYNK_PRINT Serial
#define BLYNK_USE_DIRECT_CONNECT
#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include "DHT.h"
//#include "Servo.h"

#define DHTTYPE DHT11

// Input Pins
#define PIRinputPIN 26 // entry gate
#define PIRinputPIN2 30 // exit gate 
#define BuzzerPin 13
#define DHTPin 27
//#define servoPin 25
//#define servoExit 15

//Virtual Pins 
#define temp_display_pin V1
#define gauge V0

#define temp_threshold 23
#define max_capacity 100
DHT dht(DHTPin, DHTTYPE);
//Servo Servo1;
//Servo Servo2;
int num_people;
// considering connection request through BLE
// auth token
char auth[] = "RYzAZiIej44ls7L-W6LuP0StjUULAmfN";
void setup()
{
  // Debug console
  Serial.begin(9600);
  Serial.println("Waiting for connections...");
  Blynk.setDeviceName("Blynk-Test");
  Blynk.begin(auth);
  // initialise the input pins //
  pinMode(PIRinputPIN,INPUT);
  pinMode(DHTPin, INPUT);
  pinMode(BuzzerPin,OUTPUT);
  //pinMode(servoExit,OUTPUT);
  digitalWrite(BuzzerPin,LOW);
  //Servo1.attach(servoPin);
  //Servo2.attach(servoExit);
  num_people = 0;
  dht.begin();
}
void entry_gate()
{

  bool flag = Blynk.connected();
  if(flag == true){
  // assume connection to bluetooth at entrance of the mall 
  if(num_people >= 100)
  {
    Blynk.notify("Entry Denied , max number of people reached !!");
    digitalWrite(BuzzerPin,HIGH);
    delay(2000);
    digitalWrite(BuzzerPin,LOW);
  }
  Blynk.notify("Enter the email_id through app !!");
  Blynk.notify("Check temperature");
  delay(2000);
  float temp_read = dht.readTemperature();
    Serial.println(temp_read);
     Blynk.virtualWrite(temp_display_pin, temp_read);
  if(temp_read > temp_threshold)
  {
    Blynk.notify("Entry Denied , high body temperature!");
    Blynk.virtualWrite(temp_display_pin, temp_read);
    Serial.println(temp_read);
    digitalWrite(BuzzerPin,HIGH);
    delay(2000);
    digitalWrite(BuzzerPin,LOW);
    // continue;
  } 
  Blynk.notify("Entry Success!!");
  num_people = num_people + 1;
  Blynk.virtualWrite(gauge,num_people);
  // open the gate // 
  //Servo1.write(180);
  delay(3000);
  //Servo1.write(0);
  // check number of people in the mall currently -> msg thing 
  // meter kinda thing in app ( gauge )
  // valid -> prompt to check the temperature
  // read temperature data 
  // check temperature ( if in valid range ) -> prompt entry suceess prompt servo motor and open gate
  // if entry , store the bluetooth address , update number of people , entry time (to do)
  // else prompt ghr ja rest kr
  // disconnect device
  }
  else {
    Serial.println("Waiting for device connection ~ Entry Gate");
   }
}

void exit_gate()
{  
    bool result = Blynk.connected();
    if(result == true)
    { 
      Blynk.notify("Thanks For Visiting the MAll !!");
      //Servo2.write(180);
      delay(3000); 
      //Servo2.write(0);
      num_people = num_people - 1; 
    }
}
int check_movement(){
  
  int entry_input = digitalRead(PIRinputPIN);
  if(entry_input == 1)
  {
    entry_gate();
  }
  
  int exit_output = digitalRead(PIRinputPIN2);
  
  if(exit_output == 1)
  {
    exit_gate();
  }
  
}

void loop()
{
  Blynk.run();
  check_movement();
  // Blynk.disconnect(); 
  //**EXIT**//
  // motion detect 
  // connect to bluetooth 
  // prompt to exit 
  // open the gate ( always )
  // disconnect  
}

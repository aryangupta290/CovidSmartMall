#define BLYNK_PRINT Serial
#define BLYNK_USE_DIRECT_CONNECT
#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include "DHT.h"
#include "Servo.h"

#define DHTTYPE DHT11

// Input Pins
#define PIRinputPIN 26 // entry gate
#define PIRinputPIN2 30 // exit gate 
#define BuzzerPin 27
#define DHTPin 34
#define servoPin 25
#define servoExit 15
#define whiteLED 11
//Virtual Pins 
#define temp_display_pin V1
#define gauge V0
#define temp_threshold 38
#define max_capacity 100
DHT dht(DHTPin, DHTTYPE);
Servo Servo1;
Servo Servo2;
int num_people;
// considering connection request through BLE
// auth token
char auth[] = "RYzAZiIej44ls7L-W6LuP0StjUULAmfN";
String last_message_to_send;
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
  pinMode(servoExit,OUTPUT);
  digitalWrite(BuzzerPin,LOW);
  Servo1.attach(servoPin);
  Servo2.attach(servoExit);
  num_people = 0;
  dht.begin();
}

BLYNK_WRITE(V3)
{
  String newValue = param.asString();
  if (newValue == "")
  {
    newValue = "NULL";
  }
  last_message_to_send += "Email : ";
  last_message_to_send += newValue; // name
  Blynk.notify("Entry Success!!" + newValue);
  num_people = num_people + 1;
  Blynk.virtualWrite(gauge, num_people);
  // open the gate //
  Servo1.write(180);
  delay(3000);
  Servo1.write(0);
  digitalWrite(whiteLED, HIGH);
  delay(4000);
  digitalWrite(whiteLED, LOW);
 
}

// get name
BLYNK_WRITE(V2)
{
  String newValue = param.asString();
  if (newValue == "")
  {
    newValue = "Person" + String(num_people);
  }

  Blynk.notify("Check temperature");
  delay(3000);
  float temp_read = dht.readTemperature();

  if (temp_read > temp_threshold)
  {
    Blynk.notify("Entry Denied , high body temperature!");
    Blynk.virtualWrite(temp_display_pin, temp_read);
    digitalWrite(BuzzerPin, HIGH);
    delay(4000);
    digitalWrite(BuzzerPin, LOW);
    return;
  }
  delay(1000);
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
    return;
  }
  Blynk.notify("Enter Name");
  delay(5000);
  Blynk.notify("Enter the email_id through app !!");
  delay(5000);
  
  Blynk.notify("Check temperature");
  float temp_read = dht.readTemperature();
  if(temp_read > temp_threshold)
  {
    Blynk.notify("Entry Denied , high body temperature!");
    Blynk.virtualWrite(temp_display_pin, temp_read);
    digitalWrite(BuzzerPin,HIGH);
    delay(2000);
    digitalWrite(BuzzerPin,LOW);
    return;
    // continue;
  } 
  delay(2000);
 
  Blynk.notify("Entry Success!!");
  num_people = num_people + 1;
  Blynk.virtualWrite(gauge,num_people);
  // open the gate // 
  Servo1.write(180);
  delay(3000);
  Servo1.write(0);
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
      Blynk.notify("Thanks For Visiting the Mall !!");
      Servo2.write(180);
      delay(3000); 
      Servo2.write(0);
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
  delay(3000);
}

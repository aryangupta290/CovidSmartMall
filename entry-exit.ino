//#define BLYNK_PRINT Serial
//#define BLYNK_USE_DIRECT_CONNECT
//#include <BlynkSimpleEsp32_BLE.h>
//#include <BLEDevice.h>
//#include <BLEServer.h>
#include "DHT.h"
#include "Servo.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "HTTPClient.h"
#include "time.h"
#include <ArduinoJson.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

//char auth[] = "RYzAZiIej44ls7L-W6LuP0StjUULAmfN"; 

#define DHTTYPE DHT11
#define PIRinputPIN 26  // entry gate
#define PIRinputPIN2 30 // exit gate
#define BuzzerPin 2
#define whiteLED 13
#define DHTPin 27
#define servoPin 25
//#define servoExit 15
#define exitLED 34
const int triggerPin = 15;
const int echoPin = 35;

#define c 0.034
#define convert_factor 0.393701

long duration;
float distanceCm;
float distanceInch;
//Virtual Pins
#define temp_display_pin V1
#define gauge V0
#define person_name V2
#define person_email V3
#define temp_threshold 38
#define max_capacity 100
DHT dht(DHTPin, DHTTYPE);
Servo Servo1;
Servo Servo2;
int num_people = 0;
String newValue = "";
String input = "";
int count = 0;
// considering connection request through BLE
// auth token

WebServer server(80);
AsyncWebServer serverA(81);

int authentication = 0;
bool isDetected;
float t;

String cse_ip = "192.168.108.196"; // Do ifconfig to get your ip.
String cse_port = "8080";
String Server = "http://" + cse_ip + ":" + cse_port + "/~/in-cse/in-name/";

String ae1 = "people";
String cnt1 = "node1";

String ae3 = "total_people";
String cnt3 = "node3";

String ae4 = "sanitizer";
String cnt4 = "node4";

int i = 1;

void createCIPeople(String &val)
{
  Serial.println(val);
  HTTPClient http;
  http.begin(Server + ae1 + "/" + cnt1 + "/");
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");
  int code = http.POST("{\"m2m:cin\": {\"con\": \"" + String(val) + "\",\"cnf\": \"application/json\",\"rn\": " + String(i++) + "}}");
  Serial.print("Code = ");
  Serial.println(code);
  if (code == -1)
  {
    Serial.println("UNABLE TO CONNECT TO THE SERVER");
  }
  http.end();
}

int valueTOTAL = 1;

void createCItotal_people()
{ //Serial.println(val);
  HTTPClient http;
  http.begin(Server + ae3 + "/" + cnt3 + "/");
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");

  int code = http.POST("{\"m2m:cin\": {\"con\": \"" + String(num_people) + "\",\"cnf\": \"application/json\",\"rn\": " + String(valueTOTAL++) + "}}");

  Serial.println(code);
  if (code == -1)
  {
    Serial.println("UNABLE TO CONNECT TO THE SERVER");
  }
  http.end();
}

int sanitizer_itr = 1;

void createCISanitizer(String &amount)
{
  HTTPClient http;
  http.begin(Server + ae4 + "/" + cnt4 + "/");
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");

  int code = http.POST("{\"m2m:cin\": {\"con\": \"" + amount + "\",\"cnf\": \"application/json\",\"rn\": " + String(sanitizer_itr++) + "}}");

  Serial.println(code);
  if (code == -1)
  {
    Serial.println("UNABLE TO CONNECT TO THE SERVER");
  }
  http.end();
}

//WebServer server(80);

void setup()
{
  Serial.begin(9600);
  pinMode(triggerPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  WiFi.begin("Harshita","Harshita1512");

  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.println(WiFi.status());
    delay(1000);
  }
  Serial.println(WiFi.localIP());  
  //Blynk.setDeviceName("Blynk-Test");
  //Blynk.begin(auth);

  // initialise the input pins //
  pinMode(PIRinputPIN, INPUT);
  pinMode(DHTPin, INPUT);
  pinMode(BuzzerPin, OUTPUT);
  pinMode(whiteLED, OUTPUT);
  //pinMode(servoExit, OUTPUT);
  digitalWrite(BuzzerPin, LOW);
  Servo1.attach(servoPin);
//  Servo2.attach(servoExit);
  num_people = 0;
  dht.begin();
  Serial.println(WiFi.localIP());
  
  server.begin();  
  serverA.begin();
  Serial.println("HTTP server started");

    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

  serverA.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/web.html", "text/html"); });

    // Route to load style.css file
  serverA.on("/style1.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/style1.css", "text/css"); });

  serverA.on("/person_info", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/plain", input.c_str()); });
}

float last_temperature = 0;
String last_message_to_send = "";

void entry_gate()
{
  Serial.print("num_people = ");
  Serial.println(num_people);
  
    if (num_people >= 100)
    {
      //Blynk.notify("Entry Denied , max number of people reached !!");
       Serial.println("Entry Denied , max number of people reached !!");
      digitalWrite(BuzzerPin, HIGH);
      delay(2000);
      digitalWrite(BuzzerPin, LOW);
      return;
    }
    
    Serial.println("Enter name");
    //delay(5000);
    Serial.setTimeout(5000);
    newValue = Serial.readString();
    
    //String newValue = param.asString();
    
    if (newValue == "")
    {
    newValue = "Person" + String(num_people);
    }

    //Blynk.notify("Check temperature");
    //delay(3000);
    
    float temp_read = dht.readTemperature();

    Serial.println(temp_read);
    if (temp_read > temp_threshold)
    {
    //Blynk.notify("Entry Denied , high body temperature!");
    //Blynk.virtualWrite(temp_display_pin, temp_read);
    
    digitalWrite(BuzzerPin, HIGH);
    delay(4000);
    digitalWrite(BuzzerPin, LOW);
    return;
    }

  last_temperature = temp_read;
  
  last_message_to_send = "Name : " + newValue + " Temperature : " + String(temp_read);

  count++;  
  newValue[newValue.length() - 1] = ',';
  input = String(count) + "," + newValue + String(temp_read);
  delay(1000);
  
  //Blynk.notify("Enter the email_id through app !!");
  //delay(2000);
  
  //Serial.println("Enter email");
  //Serial.setTimeout(5000);
    //newValue = Serial.readString();
  //String newValue = param.asString();
  //if (newValue == "")
  //{
    //newValue = "NULL ";
  //}
  //newValue[newValue.length() - 1] = ' ';
  
  //last_message_to_send += " Email : ";
  //last_message_to_send += newValue; // name

  //input = input + newValue;
  Serial.println(input);
  
  //Blynk.notify("Entry Success!!");
  num_people = num_people + 1;
  //Blynk.virtualWrite(gauge, num_people);
  // open the gate //
  Servo1.write(180);
  delay(1000);
  Servo1.write(0);
    digitalWrite(BuzzerPin, HIGH);
    delay(1000);
    digitalWrite(BuzzerPin, LOW);
    
  createCIPeople(input);
  createCItotal_people();

}

void exit_gate()
{  
  delay(1000);
  //bool result = Blynk.connected();
  //if (result == true)
  //{
  //Blynk.notify("Thanks For Visiting the MAll !!");
  
  digitalWrite(exitLED, HIGH);
  delay(1000);
  digitalWrite(exitLED, LOW);
  if(num_people >=1)
  num_people = num_people - 1;
  
  //}
}

void check_movement()
{
  int entry_input = digitalRead(PIRinputPIN);
  if (entry_input == 1)
  {
    entry_gate();
  }
  
//  digitalWrite(triggerPin, LOW);
//  delayMicroseconds(2);
//  digitalWrite(triggerPin, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(triggerPin, LOW);
//  duration = pulseIn(echoPin, HIGH);
//  distanceCm = duration * c/2;
//  distanceInch = distanceCm * convert_factor;
//  
//  Serial.print("Distance (cm): ");
//  Serial.println(distanceCm);
//  
//  if (distanceCm < 1)
//  {
//    exit_gate();
//  }
}

void loop()
{
  //Serial.println(WiFi.localIP());
  server.handleClient();
  check_movement();
}

/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp32-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

/*
 * 
 * Adapted by Surj Patel for CS427 Class Univ of Portland
 * 
 *
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <ESP32Servo.h>

#define STEPPER_PIN_1 23
#define STEPPER_PIN_2 22
#define STEPPER_PIN_3 18
#define STEPPER_PIN_4 5



Servo myservo;  // create servo object to control a servo
int posVal = 0;    // variable to store the servo position
int servoPin = 33; // Servo motor pin
const char* ssid = "Gogosmash";
const char* password = "jantu123";
//const char* ssid = "The Lighthouse";
//const char* password = "Monkeysex!";
int step_number = 0;
boolean active;

//Your Domain name with URL path or IP address with path
// const char* serverName = "http://192.168.1.3:5000/record/7/77";
//const char* serverName = "http://192.168.1.3:5000/record/";
const char* serverName = "https://gogoi23.pythonanywhere.com/servo/activate";
String myDeviceId = "7";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 3 seconds (3000)
unsigned long timerDelay = 3000;

String sensorReadings;
float sensorReadingsArr[3];
String finalUrl;

void setup() {
  
  // #This sets up the serial port as that's how we will "watch it working"
  // #We send messages to ourselves via it
  myservo.setPeriodHertz(50);           // standard 50 hz servo
  myservo.attach(servoPin, 500, 2500);  // attaches the servo on servoPin to the servo object
  Serial.begin(115200);

  pinMode(STEPPER_PIN_1,OUTPUT);
  pinMode(STEPPER_PIN_2,OUTPUT);
  pinMode(STEPPER_PIN_3,OUTPUT);
  pinMode(STEPPER_PIN_4,OUTPUT);

  // #start the wifi by starting it by passing the wifi name and the password to connect to your network
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    // #while it waiting to connect it will print a dot every half second
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // # notice how we build the string here in pieces, it's something that will be critical to your learning here
  Serial.println("Timer set to " + String(timerDelay) + " miliseconds (timerDelay variable), it will take that long before publishing the first reading.");
}


String httpGETRequest(const char* serverName) {
  HTTPClient http;

  // #Let's build our URL from pieces
  // #We need teh base URL
  // #Then we need to add the device ID
  // #Then we need to add a backslash 
  // #Then we need to add the rotation value, in this case randomly generated
  
  finalUrl = serverName; 

  // # Before and after lines for illustrating only
  //Serial.println("Before:  " + finalUrl);
  //finalUrl = finalUrl + myDeviceId + "/" + String(random(10,180));
  //Serial.println("After:  " + finalUrl);
  
  // Call the constructed URL, just like typing into a browser 
  http.begin(finalUrl);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 

  //#Catch and deal with Errors
  
  if (httpResponseCode>0) {
    //Serial.print("HTTP Response code: ");
    //Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}


void activateServo(int sensorReadings){
  Serial.print("sensorReadings = ");
  Serial.println(sensorReadings);
  myservo.write(sensorReadings);       
}


void loop() {
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      sensorReadings = httpGETRequest(serverName);
      Serial.print(sensorReadings);
      if (sensorReadings.toInt() == 0){
        Serial.println("Invalid Input");
        
      }
      else{  
        for(int a = 0; a < 900; a ++){
          OneStep(false);
          delay(2);
        }
        
        for(int a = 0; a < 900; a ++){
          OneStep(true);
          delay(2);
        }
        httpGETRequest("https://gogoi23.pythonanywhere.com/servo/deactivate");
          
      }
      
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}


void OneStep(bool dir){
    if(dir){
switch(step_number){
  case 0:
  digitalWrite(STEPPER_PIN_1, HIGH);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, LOW);
  break;
  case 1:
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, HIGH);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, LOW);
  break;
  case 2:
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, HIGH);
  digitalWrite(STEPPER_PIN_4, LOW);
  break;
  case 3:
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, HIGH);
  break;
} 
  }else{
    switch(step_number){
  case 0:
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, HIGH);
  break;
  case 1:
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, HIGH);
  digitalWrite(STEPPER_PIN_4, LOW);
  break;
  case 2:
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, HIGH);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, LOW);
  break;
  case 3:
  digitalWrite(STEPPER_PIN_1, HIGH);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, LOW);
 
  
} 
  }
step_number++;
  if(step_number > 3){
    step_number = 0;
  }
}

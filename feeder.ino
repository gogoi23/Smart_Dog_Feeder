/*
  This is the code for the esp32 rover module that controls the dog feeder. It controls and lcd moniter to display information, a scale
  to weigh the amount of food, and a servo motor to open and close the lid to dispense food.  
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <ESP32Servo.h>

#include "HX711.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>


#define SDA 13 //Define SDA pins
#define SCL 14 //Define SCL pins

Servo myservo; // create servo object to control a servo
int posVal = 90;// variable to store the servo position
int servoPin = 4;// Servo motor pin

const char* ssid = "Gogosmash"; //wifi network
const char* password = "jantu123";//wifi password

HX711 scale; // this is the scale that weighs how much food is in the bowl 
float calibration_factor = -434;// this is the calibaration factor for the scale. 
float units;// this variable will be used to store the amount of grams that the scale is recording


//accessing this website will active the dogfeeder. This variable is not used yet. 
const char* serverName = "https://gogoi23.pythonanywhere.com/servo/activate";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 3 seconds (3000)
unsigned long timerDelay = 3000;

//this variable is used to hold whatever accessing the servername returns. 
String sensorReadings;

//this is used to take urls as parameters and add more stuff to it 
String finalUrl;

//this initializes the lcd moniter. This displays all messages that the user should see
LiquidCrystal_I2C lcd(0x27,16,2); 


// this method is used to set up the servo, lcd and connect to the wifi before the 
// main loop starts 
void setup() {
    
  Serial.begin(115200);           // connect to the computers moniter. useful for debugging
  Wire.begin(SDA, SCL);           // attach the IIC pin
  
  //this sets up the lcd monitor 
  lcd.init();                     // LCD driver initialization
  lcd.backlight();                // Open the backlight
  lcd.setCursor(0,0);             
  
  //this sets up the servo for use 
  myservo.setPeriodHertz(50);           // standard 50 hz servo
  myservo.attach(servoPin, 500, 2500);  // attaches the servo on servoPin to the servo object

  // #start the wifi by starting it by passing the wifi name and the password to connect to your network
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  int connectingMessageCounter = 0;
  while(WiFi.status() != WL_CONNECTED) {
    // #while it waiting to connect it will print a dot every half second
    delay(500);
    lcd.clear();
    delay(500);
    lcd.setCursor(0,0); 
    
    
    if(connectingMessageCounter == 0){
        lcd.print("Connecting");
        connectingMessageCounter = 1;
    }
    
    else if(connectingMessageCounter == 1){
       lcd.print("Connecting.");
       connectingMessageCounter = 2;
    }
    
    else if(connectingMessageCounter == 2){
       lcd.print("Connecting..");
       connectingMessageCounter = 3;
    }
    
    else if (connectingMessageCounter == 3){
      lcd.print("Connecting...");
      connectingMessageCounter = 0;
    }
    
    
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connected");//prints that the device is connected to the internet on the lcd moniter for the user to see
  
  Serial.println(WiFi.localIP());

  // # notice how we build the string here in pieces, it's something that will be critical to your learning here
  Serial.println("Timer set to " + String(timerDelay) + " miliseconds (timerDelay variable), it will take that long before publishing the first reading.");
  scale.begin(25, 26);
  scale.set_scale(calibration_factor);
  scale.tare();
}

//this methods goes to whatever url servername and returns whatever the website sends back 
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



//this is the main functional code. It actively listens to https://gogoi23.pythonanywhere.com/servo/activate
//which is stored in servername. When the user enters on the wesbite that they want to feed the dog then 
// servername sends a singal
void loop() {
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      //sensor readings will equal the amount of grams of dog food that user wants
      //to put in the bowl. If the user has not inputed anything then it will equal
      //0. 
      sensorReadings = httpGETRequest(serverName);
      
      
      //this displays the amount of weight in the dog bowl measured by the scale 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(sensorReadings);
      
      //this displays on the website how much food is in the bowl measure in grams
      //the user can monitor how much food is in the bowl from miles away
      units = scale.get_units(),10;
      lcd.setCursor(0,1);
      lcd.print(String(units) + " grams");
      String unitsonline = "https://gogoi23.pythonanywhere.com/info/" + String(units);
      httpGETRequest(unitsonline.c_str());

      //if the user has not entered anything do not do anything 
      if (sensorReadings.toInt() == 0){
        //Serial.println("Invalid Input");
        
      }
      else{  
        //display on the lcd monitor that you are filling up the bowl 
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Filling Bowl ");
        
        //opens lid 
        for (posVal = 90; posVal <= 180; posVal += 1) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
          myservo.write(posVal);       // tell servo to go to position in variable 'pos'
          delay(2);                   // waits 15ms for the servo to reach the position
        }
      
        //waits until the weight is filled up
        fillUp(sensorReadings.toInt());
        
        //closes the lid 
        for (posVal = 180; posVal >= 90; posVal -= 1) { // goes from 180 degrees to 0 degrees
          myservo.write(posVal);       // tell servo to go to position in variable 'pos'
          delay(1);                   // waits 15ms for the servo to reach the position
        }
        
        //tells the website the bowl is full 
        httpGETRequest("https://gogoi23.pythonanywhere.com/servo/deactivate");
          
      }
      
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

//this method waignts until the scale's weight is equal to or higher than the curent weight.  
void fillUp(int weight){
  boolean open = true;
  while(open){
    Serial.print("Reading:");
    
    units = scale.get_units(),10;
    if (units < 0){
      units = 0.00;
    }
    
    Serial.print(units);
    Serial.println(" grams");
    lcd.setCursor(0,1);
    lcd.print(String(units) + "/" + String(weight) +  "grams");
    if(units >= weight){
      open = false;
    }
    delay(1000);
  }
}

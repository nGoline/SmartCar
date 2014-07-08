/*
AUTHOR: Níckolas Goline (nGoline)
DATE: 2014 07 02
CONTACT: n@ngoline.com
*/

#include <Wire.h>
#include <SoftwareSerial.h>

#define RX 9
#define TX 10
#define CONTROL 8
#define ROLE 7
#define LED 11
#define RESET 6
#define CONNECTED 5

byte STEP = 0;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

SoftwareSerial BTSerial(RX, TX);

void setupBLE() {
  //set the pins IO
  pinMode(LED,OUTPUT);
  pinMode(ROLE,OUTPUT);
  pinMode(CONTROL,OUTPUT);
  pinMode(RESET,OUTPUT);
  pinMode(CONNECTED,INPUT);
  
  //Set Role to peripheral
  digitalWrite(ROLE,HIGH);
  //Set Control to Transparent Data Mode
  digitalWrite(CONTROL,HIGH);

  //Reset the device
  digitalWrite(RESET,LOW);
  delay(100);
  digitalWrite(RESET,HIGH);  

  Serial.begin(9600);
  BTSerial.begin(9600);
  Serial.println("Waiting Connection");
}

void checkConnection() {
  if(digitalRead(CONNECTED) == HIGH) {
    STEP = 1;
    Serial.println("Connected");
  }
}

void waitAuthentication() {
  if (BTSerial.available()) Serial.println("there's data");
  if (!stringComplete) {
  softwareSerialEvent();
  } else {
    Serial.println(inputString);
  }  
}

void softwareSerialEvent() {  
  while (BTSerial.available()) {
    // get the new byte:
    char inChar = (char)BTSerial.read(); 
    Serial.println(inChar);
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}

void setup() {
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  // setup the ble serial connection
  setupBLE();
}

void loop() {
  if (BTSerial.available())
    Serial.write(BTSerial.read());
}

void loop2() {
  //set the step back to 0 if the connection is lost
  if(STEP > 0 && digitalRead(CONNECTED) == LOW) {
    STEP = 0;
    Serial.println("Disconnected");
  }
  
  //check which step it is in
  switch(STEP) {
    case 0:
      checkConnection();
      break;
    case 1:
      waitAuthentication();
      break;
  }
}

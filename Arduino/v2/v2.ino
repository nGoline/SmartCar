/*
AUTHOR: Níckolas Goline (nGoline)
DATE: 2014 07 02
CONTACT: n@ngoline.com

The MIT License (MIT)

Copyright (c) <year> <copyright holders>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
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

/*
AUTHOR: Níckolas Goline (nGoline)
DATE: 2015 02 05
CONTACT: n@ngoline.com

The MIT License (MIT)

Copyright (c) 2014 Níckolas Goline

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

/*************************************************************************/
/* Global Variables */
/*************************************************************************/
const char password[] = {54, 52, 50, 53}; //6425
const char startCode[] = {50, 52, 54, 57}; //2469
const char openCode[] = {54, 55, 51, 54}; //6736
const char closeCode[] = {50, 53, 55, 51}; //2573

byte _step = 0;
long _challenge = 0;
/*************************************************************************/
void setup(){
  randomSeed(analogRead(0));
  
  Serial.begin(57600);  
  Serial.setTimeout(25);
}
/*************************************************************************/
void loop(){
  if(Bean.getConnectionState())
    Bean.setLed(0, 0, 255);
  else
    Bean.setLed(0, 0, 0);
    
  
  char buffer[10];
  size_t length = 10;
  size_t command = 0;
  boolean valid = false;
  
  length = Serial.readBytes(buffer, length);  
  if( length > 0 ){
    if (checkChallenge(buffer) || _step == 0){
      int count = 0;
      valid = true;
      while (count < 4){
        if (_step == 0){
          if (buffer[count] != password[count]){
            valid = false;
            break;
          }          
        }
        else if (_step == 1)
        {
          if(buffer[count + 1] != startCode[count]){
            valid = false;
            break;
          }
        }else{
          if (command == 0){
            command = 1;
            count = 1;
          }
          else if (command == 1 && buffer[count + 1] != openCode[count]){
            command = 2;
            count = 1;
          }
          else if (command == 2 && buffer[count + 1] != closeCode[count]){
            valid = false;
            break;
          }
        }

        count++;
      }
      
      if (valid){
        if (_step == 0 || _step == 1){
          getChallenge();
          _step++;
        }
        else{
          if (command == 1)
            openCar();
          else
            closeCar();

          _step = 0;
          _challenge = 944628;
          command = 0;
        }
      } else _step = 0;
    } else _step = 0;
  }
    
  Bean.sleep(0xFFFFFFFF); // Sleep until a serial message wakes us up
}

void getChallenge(){
  //min value = 10
  //max value = 82
  long randNumber = random(365555, 999999);  
  _challenge = ((((randNumber/6)+4531)/2)-548)/1000;
  while(_challenge > 99){
    _challenge = _challenge/10;
  }
  
  Serial.println(randNumber);
}

static bool checkChallenge(char* buffer)
{
  return _challenge == (int)buffer[0];
}

void openCar()
{
  Bean.setLed(0, 255, 0);
  delay(250);
  Bean.setLed(0, 0, 0);
}

void closeCar()
{
  Bean.setLed(255, 0, 0);
  delay(250);
  Bean.setLed(0, 0, 0);
}

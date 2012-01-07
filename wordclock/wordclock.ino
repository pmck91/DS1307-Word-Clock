/*________________________________________________
 *|                                               \
 *|                                                \
 *|   D S 1 3 0 7   W O R D   C L O C K  - V1.0     \
 *|                                                  \
 *|                                                   \
 *|--------------------------------------------------|*
 *| COPYRIGHT   : (C)2012 Peter McKinney @           |*
 *|-------------: peter@pmck.info                    |*
 *|--------------------------------------------------|*
 *| AUTHOR      : Peter McKinney                     |*
 *|--------------------------------------------------|*
 *| REFRENCES   : Roughly bassed on drj113's code @  |*
 *|-------------: http://tinyurl.com/yg3jw94         |*
 *|-------------: and scottbez1's design and code @  |*
 *|-------------: http://tinyurl.com/7luakxx         |*
 *|--------------------------------------------------|*
 *| DESCRIPTION : An Arduino based clock using leds  |*
 *|-------------: behind a printed letter mask to    |*
 *|-------------: display the time using words.      |*
 *|-------------: uses the adafruit's RTClib found   |*
 *|-------------: here  @                            |*___________
 *|-------------: https://github.com/adafruit/RTClib |*           \
 *|--------------------------------------------------|*            \ 
 *| HARDWARE    : For list go to                     |*             \
 *|-------------: http://tinyurl.com/7yuwbjk         |*              \
 *|--------------------------------------------------|*               \
 *| TUTORIAL    : For build instructions go to       |*                \
 *|-------------: http://tinyurl.com/6uqfwya         |*                 \
 *|--------------------------------------------------|*                  \
 *|                   __LICENCE__                    |*                   \
 *|----------------------------------------------------------------------|*
 *|This program is free software; you can redistribute it and/or modify  |*
 *|it under the terms of the GNU General Public License as published by  |*
 *| the Free Software Foundation; either version 2 of the License, or    |*
 *| (at your option) any later version.                                  |*
 *|                                                                      |*
 *| This program is distributed in the hope that it will be useful,      |*
 *| but WITHOUT ANY WARRANTY; without even the implied warranty of       |*
 *| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |*
 *| GNU General Public License for more details.                         |*
 *|                                                                      |*
 *| You should have received a copy of the GNU General Public License    |*
 *| along with this program; if not, write to the Free Software          |*
 *| Foundation, Inc., 59 Temple Place, Suite 330, Boston.                |*
 *| Please see LICENCE file for full licence                             |*
 *|----------------------------------------------------------------------|*/ 

//--------- INCLUDES ---------//
#include <Wire.h>
#include <RTClib.h>


// confused by whats in the following defines? this should help -> http://tinyurl.com/glb6p
//--------- MINUTE DISPLAY CONTROL --------//
#define MTEN 	Display1=Display1 | (1<<0)  
#define HALF	Display1=Display1 | (1<<1)
#define QUARTER	Display1=Display1 | (1<<2)
#define TWENTY	Display1=Display1 | (1<<3)
#define MFIVE	Display1=Display1 | (1<<4)
#define MINUTES	Display1=Display1 | (1<<5)
#define PAST	Display1=Display1 | (1<<6)
#define A	Display1=Display1 | (1<<7)

//----------- HOUR DISPLAY ONE ------------//
#define TO	Display2=Display2 | (1<<0)
#define ONE	Display2=Display2 | (1<<1)
#define TWO	Display2=Display2 | (1<<2)
#define THREE	Display2=Display2 | (1<<3)
#define FOUR	Display2=Display2 | (1<<4)
#define HFIVE	Display2=Display2 | (1<<5)
#define SIX	Display2=Display2 | (1<<6)
#define UNUSED2	Display2=Display2 | (1<<7)

//------------ HOUR DISPLAY TWO ------------//
#define SEVEN	Display3=Display3 | (1<<0)
#define EIGHT	Display3=Display3 | (1<<1)
#define NINE	Display3=Display3 | (1<<2)
#define HTEN	Display3=Display3 | (1<<3)
#define ELEVEN	Display3=Display3 | (1<<4)
#define TWELVE	Display3=Display3 | (1<<5)
#define OCLOCK  Display3=Display3 | (1<<6)
#define UNUSED3	Display3=Display3 | (1<<7)

#define MINBRIGHTNESS 85 // 1/3 brightness

// SHIFT REGISTER PINS
#define LCP 6 // led clock pin
#define LDP 7 // led data pin
#define LSP 8 // led strobe pin

// BUTTON PINS
#define MBP 2 // minute button
#define HBP 3 // hour button
#define DSP 4 // daylight savings

// BRIGHTNESS PINS
#define BCP A0 // brightness input pin
#define BSP 9  // brightness output pin

RTC_DS1307 RTC; // RTC class

uint16_t year; // 16bbit year
uint8_t month; // 8bit month
uint8_t day;   // 8bit month
uint8_t hour;  // 8bit hour
uint8_t min;   // 8bit minute
uint8_t sec;   // 8bit second

DateTime timeNow; // will store current time

boolean testMode = false;
boolean dayLightSavings = false; // +1hour if true 

char Display1=0, Display2=0, Display3=0; // the three display shifters each 8 bits long

int brightnessLevel = 255; // sotres how bright the display should be

void setup(){
  
  pinMode(LCP, OUTPUT); 
  pinMode(LDP, OUTPUT); 
  pinMode(LSP, OUTPUT);
  
  pinMode(13, OUTPUT);
  
  pinMode(MBP, INPUT); 
  pinMode(HBP, INPUT);
  pinMode(DSP, INPUT);
  digitalWrite(MBP, HIGH);  
  digitalWrite(HBP, HIGH); 
  digitalWrite(DSP, HIGH);
  
  pinMode(BSP, OUTPUT); 
  
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
  
  timeNow = RTC.now();
  
  displaytime();   
}


void ledsoff(void) {
  
  // zero the displays bits
  Display1=0;
  Display2=0;
  Display3=0;
}


void WriteLEDs(void) {
  
  // shift out display 1 to 3
  shiftOut(LDP, LCP, MSBFIRST, Display3);
  shiftOut(LDP, LCP, MSBFIRST, Display2);
  shiftOut(LDP, LCP, MSBFIRST, Display1);
  digitalWrite(LSP,HIGH);
  delay(2);
  digitalWrite(LSP,LOW); 
}


void displayTest(void){
  
  //runs a test on the display to check all LEDs are working
  Serial.println("\nTEST\n");
  analogWrite(BSP, 255);
  
  ledsoff(); MTEN; WriteLEDs(); delay(500); 
  ledsoff(); HALF; A; WriteLEDs(); delay(500); 
  ledsoff(); A; QUARTER; WriteLEDs(); delay(500); 
  ledsoff(); TWENTY; WriteLEDs(); delay(500); 
  ledsoff(); MFIVE; WriteLEDs(); delay(500); 
  ledsoff(); MINUTES; WriteLEDs(); delay(500); 
  ledsoff(); PAST; WriteLEDs(); delay(500); 
  ledsoff(); TO; WriteLEDs(); delay(500); 
  ledsoff(); ONE; WriteLEDs(); delay(500); 
  ledsoff(); TWO; WriteLEDs(); delay(500); 
  ledsoff(); THREE; WriteLEDs(); delay(500); 
  ledsoff(); FOUR; WriteLEDs(); delay(500); 
  ledsoff(); HFIVE; WriteLEDs(); delay(500); 
  ledsoff(); SIX; WriteLEDs(); delay(500); 
  ledsoff(); SEVEN; WriteLEDs(); delay(500); 
  ledsoff(); EIGHT; WriteLEDs(); delay(500); 
  ledsoff(); NINE; WriteLEDs(); delay(500); 
  ledsoff(); HTEN; WriteLEDs(); delay(500); 
  ledsoff(); ELEVEN; WriteLEDs(); delay(500); 
  ledsoff(); TWELVE; WriteLEDs(); delay(500); 
  ledsoff(); OCLOCK; WriteLEDs(); delay(500);
  
  ledsoff(); analogWrite(BSP, 255);
  
  MTEN; HALF; A; QUARTER; TWENTY; MFIVE;
  MINUTES; PAST; TO; ONE; TWO; THREE;
  FOUR; HFIVE; SIX; SEVEN; EIGHT; NINE; 
  HTEN; ELEVEN; TWELVE; OCLOCK; 
  
  WriteLEDs(); 
  
  for(int i = 255; i >= 0; i--){
    analogWrite(BSP,i);
    delay(40);
  }
  for(int i = 0; i < 255; i++){
    analogWrite(BSP,i);
    delay(40);
  }
  
  ledsoff();
  
  digitalWrite(13,HIGH);
  delay(500);
  digitalWrite(13,LOW);
  delay(500);
  digitalWrite(13,HIGH);
  delay(500);
  digitalWrite(13,LOW);
  delay(500);
  digitalWrite(13,HIGH);
  delay(500);
  digitalWrite(13,LOW);
} 


void displaytime(void){
  
  // turn off all LEDs
  ledsoff();

  Serial.print("It is ");

// display the appropriate minute counter
  if ((timeNow.minute()>4) && (timeNow.minute()<10)) { 
    MFIVE; 
    MINUTES; 
    Serial.print("Five Minutes ");
  } 
  if ((timeNow.minute()>9) && (timeNow.minute()<15)) { 
    MTEN; 
    MINUTES; 
    Serial.print("Ten Minutes ");
  }
  if ((timeNow.minute()>14) && (timeNow.minute()<20)) {
    A; QUARTER; 
    Serial.print("A Quarter ");
  }
  if ((timeNow.minute()>19) && (timeNow.minute()<25)) { 
    TWENTY; 
    MINUTES; 
    Serial.print("Twenty Minutes ");
  }
  if ((timeNow.minute()>24) && (timeNow.minute()<30)) { 
    TWENTY; 
    MFIVE; 
    MINUTES;
    Serial.print("Twenty Five Minutes ");
  }  
  if ((timeNow.minute()>29) && (timeNow.minute()<35)) {
    HALF; A;
    Serial.print("Half ");
  }
  if ((timeNow.minute()>34) && (timeNow.minute()<40)) { 
    TWENTY; 
    MFIVE; 
    MINUTES;
    Serial.print("Twenty Five Minutes ");
  }  
  if ((timeNow.minute()>39) && (timeNow.minute()<45)) { 
    TWENTY; 
    MINUTES; 
    Serial.print("Twenty Minutes ");
  }
  if ((timeNow.minute()>44) && (timeNow.minute()<50)) {
    A; QUARTER; 
    Serial.print("A Quarter ");
  }
  if ((timeNow.minute()>49) && (timeNow.minute()<55)) { 
    MTEN; 
    MINUTES; 
    Serial.print("Ten Minutes ");
  } 
  if (timeNow.minute()>54) { 
    MFIVE; 
    MINUTES; 
    Serial.print("Five Minutes ");
  }

  // display the correct hour depending on how close we are to the next or last
  if ((timeNow.minute() <5)){
    
    switch (timeNow.hour()) {
      case 1:
      case 13:	
        ONE; 
        Serial.print("One ");
        break;
      case 2: 
      case 14:
        TWO; 
        Serial.print("Two ");
        break;
      case 3: 
      case 15:
        THREE; 
        Serial.print("Three ");
        break;
      case 4: 
      case 16:
        FOUR; 
        Serial.print("Four ");
        break;
      case 5: 
      case 17:
        HFIVE; 
        Serial.print("Five ");
        break;
      case 6: 
      case 18:
        SIX; 
        Serial.print("Six ");
        break;
      case 7: 
      case 19:
        SEVEN; 
        Serial.print("Seven ");
        break;
      case 8: 
      case 20:
        EIGHT; 
        Serial.print("Eight ");
        break;
      case 9: 
      case 21:
        NINE; 
        Serial.print("Nine ");
        break;
      case 10:
      case 22:	
        HTEN; 
        Serial.print("Ten ");
        break;
      case 11:
      case 23:	
        ELEVEN; 
        Serial.print("Eleven ");
        break;
      case 12:
      case 0:	
        TWELVE; 
        Serial.print("Twelve ");
        break;
    }
    OCLOCK;
    Serial.println("O'Clock");
  }
  
  else if ((timeNow.minute() < 35) && (timeNow.minute() >4)){
    
    PAST;
    Serial.print("Past ");
    switch (timeNow.hour()) {
      case 1:
      case 13: 
        ONE; 
        Serial.println("One ");
        break;
      case 2:
      case 14: 
        TWO; 
        Serial.println("Two ");
        break;
      case 3:
      case 15: 
        THREE; 
        Serial.println("Three ");
        break;
      case 4:
      case 16: 
        FOUR; 
        Serial.println("Four ");
        break;
      case 5:
      case 17: 
        HFIVE; 
        Serial.println("Five ");
        break;
      case 6:
      case 18: 
        SIX; 
        Serial.println("Six ");
        break;
      case 7:
      case 19: 
        SEVEN; 
        Serial.println("Seven ");
        break;
      case 8:
      case 20: 
        EIGHT; 
        Serial.println("Eight ");
        break;
      case 9:
      case 21: 
        NINE; 
        Serial.println("Nine ");
        break;
      case 10:
      case 22: 
        HTEN; 
        Serial.println("Ten ");
        break;
      case 11:
      case 23: 
        ELEVEN; 
        Serial.println("Eleven ");
        break;
      case 12:
      case 0: 
        TWELVE; 
        Serial.println("Twelve ");
        break;
    }
  }
  
  else{
    
    TO;
    Serial.print("To ");
    switch (timeNow.hour()) {
      case 1: 
      case 13:
        TWO; 
        Serial.println("Two ");
        break;
      case 2: 
      case 14:
        THREE; 
        Serial.println("Three ");
        break;
      case 3: 
      case 15:
        FOUR; 
        Serial.println("Four ");
        break;
      case 4: 
      case 16:
        HFIVE; 
        Serial.println("Five ");
        break;
      case 5: 
      case 17:
        SIX; 
        Serial.println("Six ");
        break;
      case 6: 
      case 18:
        SEVEN; 
        Serial.println("Seven ");
        break;
      case 7: 
      case 19:
        EIGHT; 
        Serial.println("Eight ");
        break;
      case 8: 
      case 20:
        NINE; 
        Serial.println("Nine ");
        break;
      case 9: 
      case 21:
        HTEN; 
        Serial.println("Ten ");
        break;
      case 10:
      case 22:	  
        ELEVEN; 
        Serial.println("Eleven ");
        break;
      case 11:
      case 23:
        TWELVE; 
        Serial.println("Twelve ");
        break;
      case 12:
      case 0:	  
        ONE; 
        Serial.println("One ");
        break;
    }
  }

  WriteLEDs();

}


void loop(void)
{

  if(testMode){
    displayTest();
    delay(1000);
  }

  else{
  
    timeNow = RTC.now(); // get the time

    Serial.println();
    Serial.println();
    Serial.print(timeNow.day(), DEC);
    Serial.print('/');
    Serial.print(timeNow.month(), DEC);
    Serial.print('/');
    Serial.print(timeNow.year(), DEC);
    Serial.print(' ');
    Serial.print(timeNow.hour(), DEC);
    Serial.print(':');
    Serial.print(timeNow.minute(), DEC);
    Serial.print(':');
    Serial.print(timeNow.second(), DEC);
    Serial.println();
    Serial.println();
  
    
    //brightnessLevel = analogRead(BCP);  // read the brightness level      
   // brightnessLevel = map(brightnessLevel,0,1023,0,255); // map it to the proper range
   // if(brightnessLevel < MINBRIGHTNESS) brightnessLevel = MINBRIGHTNESS; // adjust it if its below the min brightness
    analogWrite(BSP, 255); // make the display that bright
    
  
    displaytime(); 
    
    if(digitalRead(MBP) ==0 && digitalRead(HBP)!=0 && digitalRead(DSP)!=0 ) {
      Serial.println("minute ++");
      
      //add one minute
      year = timeNow.year();
      month = timeNow.month(); 
      day = timeNow.day();
      hour = timeNow.hour(); 
      min = timeNow.minute();
      sec = 0;
      
      if(++min >= 60){
        min = 0;
      }
      
      DateTime timeToSet(year,month,day,hour,min,sec);
      
      RTC.adjust(timeToSet);
      
      delay(500); // allow user to remove finger from button
      displaytime();
    }
    
    if(digitalRead(HBP)==0 && digitalRead(MBP)!=0 && digitalRead(DSP)!=0 ){
      Serial.println("Hour ++");
      
      // add one hour
      year = timeNow.year();
      month = timeNow.month(); 
      day = timeNow.day();
      hour = timeNow.hour(); 
      min = timeNow.minute();
      sec = 0;
      
      if(++hour >= 24){
        hour = 0;
      }
      
      DateTime timeToSet(year,month,day,hour,min,sec);
      
      RTC.adjust(timeToSet);
      
      delay(500); // allow user to remove finger from button
      displaytime();
    }
    
    if(digitalRead(DSP)==0 && digitalRead(MBP)!=0 && digitalRead(HBP)!=0 ){
      Serial.print("Daylight savigns are enabled (plus one hour if yes)?  ->  ");
      Serial.println(dayLightSavings);
      
      // add or remove one hour based on daylight savings
      year = timeNow.year();
      month = timeNow.month(); 
      day = timeNow.day();
      hour = timeNow.hour(); 
      min = timeNow.minute();
      sec = 0;
      
      dayLightSavings = !dayLightSavings;
      
      if(dayLightSavings){
          if(++hour == 24){
            hour = 0;
          }
      }
      else{
        if(hour-- == 0){
            hour = 23;
        }
      }
      
      DateTime timeToSet(year,month,day,hour,min,sec);
      
      RTC.adjust(timeToSet);
      
      delay(500);
      displaytime();
    }
  
  }
  
  if (digitalRead(HBP)==0 && digitalRead(MBP)==0 ){
   testMode = !testMode;
   delay(5000); // stop user entering test mode straight after exiting it 
  }
  
}		  



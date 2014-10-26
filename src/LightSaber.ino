#include "pitches.h"

// ---------------- Accelerometer - BEGIN ---------------- //

#include <Wire.h>

#define BMA180 0x40  //address of the accelerometer
#define RESET 0x10 
#define PWR 0x0D
#define BW 0X20
#define RANGE 0X35
#define DATA 0x02

int offx = 31; 
int offy = 47; 
int offz = -23;

// ---------------- Accelerometer - END ---------------- //

const int  buttonPin = 2;    // the pin that the pushbutton is attached to
const int ledPin = 9;        // the pin that the LED is attached to
const int speakerPin = 8;    // the pin that the speaker is attached to

int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

int noteDuration = 200;
int brightness = 0;
int fadeAmount = 5;
int isOn = 0;

// ---------------- Accelerometer - BEGIN ---------------- //

int isFirstTime = 1;
float previousX;
float previousY;
float previousZ;

//Writes val to address register on ACC
void writeTo(int DEVICE, byte address, byte val) {
  Wire.beginTransmission(DEVICE);   //start transmission to ACC
  Wire.write(address);               //send register address
  Wire.write(val);                   //send value to write
  Wire.endTransmission();           //end trnsmisson
}

//reads num bytes starting from address register in to buff array
void readFrom(int DEVICE, byte address , int num ,byte buff[]) {
  Wire.beginTransmission(DEVICE); //start transmission to ACC
  Wire.write(address);            //send reguster address
  Wire.endTransmission();        //end transmission

  Wire.beginTransmission(DEVICE); //start transmission to ACC
  Wire.requestFrom(DEVICE,num);  //request 6 bits from ACC

  int i = 0;
  while(Wire.available())        //ACC may abnormal
  {
    buff[i] =Wire.read();        //receive a byte
    i++;
  }
  Wire.endTransmission();         //end transmission
}

void AccelerometerInit() {
  byte temp[1];
  byte temp1;
  
  writeTo(BMA180,RESET,0xB6);
  //wake up mode
  writeTo(BMA180,PWR,0x10);
  // low pass filter,
  readFrom(BMA180, BW,1,temp);
  temp1=temp[0]&0x0F;
  writeTo(BMA180, BW, temp1); 
  // range +/- 2g
  readFrom(BMA180, RANGE, 1 ,temp); 
  temp1=(temp[0]&0xF1) | 0x04;
  writeTo(BMA180,RANGE,temp1);
}

void AccelerometerRead()
{
  // read in the 3 axis data, each one is 14 bits
  int n = 6;
  byte result[5];
  readFrom(BMA180, DATA, n , result);

  int x = (( result[0] | result[1]<<8)>>2)+offx;
  float x1 = x/4096.0;
 
  int y = (( result[2] | result[3]<<8 )>>2)+offy;
  float y1 = y/4096.0;
 
  int z = (( result[4] | result[5]<<8 )>>2)+offz;
  float z1 = z/4096.0;
  
  if (isFirstTime == 1)
  {
    isFirstTime = 0;
    previousX = x1;
    previousY = y1;
    previousZ = z1;
  }
  
  float diffX = x1 - previousX;
  float diffY = y1 - previousY;
  float diffZ = z1 - previousZ;
  
  float absX = abs(diffX);
  float absY = abs(diffY);
  float absZ = abs(diffZ);

  if (absX > 0.70 || absY > 0.70 || absZ > 0.70) {
    Serial.println("wykraczam solidnie!!!");
    tone(speakerPin, NOTE_DS3);
  }
  else if (absX > 0.30 || absY > 0.30 || absZ > 0.30) {
    Serial.println("wykraczam");
    tone(speakerPin, NOTE_CS3);
  }
  else {
    tone(speakerPin, NOTE_AS2);
  }
  
  previousX = x1;
  previousY = y1;
  previousZ = z1;
}

// ---------------- Accelerometer - END ---------------- //

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  Serial.begin(9600);

  Wire.begin();
  Serial.println("Initializing accelerometer's sensors.");
  AccelerometerInit();
  Serial.println("Sensors have been initialized.");
}

int melody[] = {
  NOTE_B0, NOTE_B0,
  NOTE_C1, NOTE_C1,
  NOTE_CS1, NOTE_CS1,
  NOTE_D1, NOTE_D1,
  NOTE_DS1, NOTE_DS1,
  NOTE_E1, NOTE_E1,
  NOTE_F1, NOTE_F1,
  NOTE_FS1, NOTE_FS1,
  NOTE_G1, NOTE_G1,
  NOTE_GS1, NOTE_GS1,
  NOTE_A1, NOTE_A1,
  NOTE_AS1, NOTE_AS1,
  NOTE_B1, NOTE_B1,
  NOTE_C2, NOTE_C2,
  NOTE_CS2, NOTE_CS2,
  NOTE_D2, NOTE_D2,
  NOTE_DS2, NOTE_DS2,
  NOTE_E2, NOTE_E2,
  NOTE_F2, NOTE_F2,
  NOTE_FS2, NOTE_FS2,
  NOTE_G2, NOTE_G2,
  NOTE_GS2, NOTE_GS2,
  NOTE_A2, NOTE_A2,
  NOTE_AS2, NOTE_AS2, NOTE_AS2, NOTE_AS2, NOTE_AS2
};

int melodyTurnOff[] = {};

void turnOn() {
  int note = 0;
  while (brightness < 255) {
    brightness = brightness + fadeAmount;
    analogWrite(ledPin, brightness);
    tone(speakerPin, melody[note++], noteDuration);
    delay(40);
    noTone(speakerPin);
  }
  Serial.println("Turning on - completed");
  
  tone(speakerPin, melody[50]);
}

void turnOff() {
  int note = 50;
  while (brightness > 0) {
    brightness = brightness - fadeAmount;
    analogWrite(ledPin, brightness);
    tone(speakerPin, melody[note--], noteDuration);
    delay(20);
    noTone(speakerPin);
  }
  Serial.println("Turning off - completed");
}

void loop() {
  buttonState = digitalRead(buttonPin);
  
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      if (isOn == 0) {
        Serial.println("Turning on");
        isOn = 1;
        turnOn();
      } else {
        Serial.println("Turning off");
        isOn = 0;
        turnOff();
      }
    }
  }
  lastButtonState = buttonState;
  
  if (isOn == 1)
  {
    AccelerometerRead();
    delay(100);
  }
}


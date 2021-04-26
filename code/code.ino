/*
Author: Aditya Dalvi
Date:24/2/2021
Name: code.ino
Project: Agrosmart
*/

#include <Wire.h>
#include <DS3231.h>
#include <SPI.h>
#include <SD.h>
#define numOfSensors 2

DS3231 rtc(SDA, SCL);

int val[numOfSensors];
Time t;

const int ledPin = 2;
int ledState = LOW;
unsigned long previousMillis = 0;
bool SDInit = true;
void setup() {
  delay(2000);
  Serial.begin(9600);
  rtc.begin();
  pinMode(3, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  Serial.println("Setup finished");
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    SDInit = false;
  }else{
    Serial.println("initialization done.");
  }
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print(" v2 start: ");
    dataFile.print(rtc.getDateStr());
    dataFile.println(rtc.getTimeStr());
    dataFile.close();
    Serial.println("written");
  } else {
    Serial.println("error opening datalog.txt");
  }
}

void loop() {
  int sum =0 , avg =0;
  t = rtc.getTime();
  val[0] = map(analogRead(A0), 700, 200, 0, 100);
  val[1] = map(analogRead(A1), 700, 200, 0, 100);

  for(int i=0; i < numOfSensors; i++){
    sum = sum + val[i];
  }
  avg = sum/numOfSensors;
  
  String PrintString = rtc.getTimeStr() + String(" ") + rtc.getDateStr();
  for(int j=0; j<numOfSensors; j++){
    PrintString += String(" val")+String(j)+String(": ")+String(val[j]);
  }
  PrintString += String(" avg:") + String(avg);

  Serial.println(PrintString);
  if (/*t.min == 00 && */t.sec == 00 && SDInit) {
    Serial.println("Logging");
    digitalWrite(2, HIGH);
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println(PrintString);
      dataFile.close();
      Serial.println("written");
      delay(1000);
      digitalWrite(2, LOW);
    } else {
      Serial.println("error opening datalog.txt");
      delay(500);
      digitalWrite(2, LOW);
      delay(500);
      digitalWrite(2, HIGH);
      delay(500);
      digitalWrite(2, LOW);
    }
  }

  if (/*t.hour == 10 && t.min == 00 && */t.sec == 10) {
    Serial.print("water ");
    Serial.println(avg);
    water(avg);
  }
  delay(1000);
}
void water(int avg) {
  int duration = map(avg, 0 , 100 , 10 , 1 );
  digitalWrite(3, LOW);
  if (duration < 0) {
    duration = 0;
  }
  Serial.print("starting watering, duration = ");
  Serial.println(duration);
  delay(duration * 1000);
  digitalWrite(3, HIGH);
  Serial.println(".....done watering");
}

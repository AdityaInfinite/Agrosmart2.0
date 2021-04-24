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

DS3231 rtc(SDA, SCL);

int val0 = 0, val1 = 0, avg = 0;
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
  digitalWrite(3, HIGH);
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
  t = rtc.getTime();
  val0 = map(analogRead(A0), 700, 200, 0, 100);
  val1 = map(analogRead(A1), 700, 200, 0, 100);

  avg = (val0 + val1) / 2;

  String TimeString = rtc.getTimeStr(); TimeString += " ";
  TimeString += rtc.getDateStr(); TimeString += " ";
  Serial.print(TimeString); Serial.print(" | ");
  Serial.print(val0); Serial.print(" ");
  Serial.print(val1); Serial.print(" | ");
  Serial.println(avg);
  if (t.sec == 00 && SDInit) {
    Serial.println("Logging");
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.print(TimeString);
      dataFile.print(" val0: ");
      dataFile.print(val0);
      dataFile.print(" val1: ");
      dataFile.print(val1);
      dataFile.print(" avg: ");
      dataFile.println(avg);
      dataFile.close();
      Serial.println("written");
      delay(1000);
    } else {
      Serial.println("error opening datalog.txt");
    }
  }

  if (/*t.hour == 10 && t.min == 11 && */t.sec == 30) {
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
void blink(int interval) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);
  }
}

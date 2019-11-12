#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  initSD();
  //initIMU();
  //initGPS();
  printLineToFile("memes");
  printLineToFile("testing");
  printLineToFile("file writing and stuff \n new line?");
  printLineToFile("end of file");
  printFileToSerial();
  printLineToFile("testing");
  printFileToSerial();
}

void loop() {
  //printIMUData();
  //readGPS();
  delay(1000);
  //Serial.println("b");
}

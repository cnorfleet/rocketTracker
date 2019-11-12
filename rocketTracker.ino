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
  initIMU();
  //initGPS();
  delay(1000); // for IMU startup
}

void loop() {
  //printIMUData();
  printLineToFile(getIMUDataStr());
  saveSDFile(); // temporary workaround because apparently the file only saves if it gets closed
  //readGPS();
  delay(100);
}

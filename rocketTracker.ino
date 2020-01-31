#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Adafruit_GPS.h>

#define PRINT_TO_SERIAL true // for debug

struct gpsTimeType {
  float s;
  int m, h, d, mth, yr;
};

struct gpsStateType {
  struct gpsTimeType gpsTime;
  String date;
  String timeUTC;
  
  unsigned int satFix;
  unsigned int quality;
  unsigned long secondsSinceFix;
  int numSats; // number of satellites

  String location;
  float latitude;
  char lat; // either N or S
  float longitude;
  char lon; // either W or E
  float speedKnots; // knots
  float angle;      // degrees
  float altitude;   // meters I think?
};

struct threeAxisDataType {
  double x, y, z;
};

struct imuStateType {
  struct threeAxisDataType orientation;
  struct threeAxisDataType angVel;
  struct threeAxisDataType accelWithGravity;
  struct threeAxisDataType linearAccel; // w/o gravity
  int8_t temperature; // board temp
};

struct rocketStateType {
  struct gpsStateType gpsState;
  struct imuStateType imuState;
} rocketState;

void setup() {
  initSerialPrinter();
  initSD();
  initIMU();
  initGPS();
  delay(1000); // for IMU startup
}

unsigned long lastTime = millis();
boolean printedSerialYet = false;

void loop() {
  updateGPSData(rocketState.gpsState);
  updateIMUData(rocketState.imuState);

  unsigned long currentTime = millis();
  if(PRINT_TO_SERIAL and ((currentTime-lastTime)>50) and (not printedSerialYet)) {
    printStatusToSerial(rocketState);
    printedSerialYet = true;
  } else if ((currentTime-lastTime)>100) {
    printStatusToFile(rocketState);
    lastTime = currentTime;
    printedSerialYet = false;
  }
}

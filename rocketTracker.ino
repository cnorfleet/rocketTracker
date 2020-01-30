#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Adafruit_GPS.h>

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
  
  // general info
} rocketState;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  initSD();
  initIMU();
  initGPS();
  delay(1000); // for IMU startup
}

void loop() {
  //printLineToFile(getIMUDataStr());
  updateGPSData(rocketState.gpsState);
  updateIMUData(rocketState.imuState);
  //printIMUData();
  printStatusToSerial(rocketState);
  //printStatusToFile(rocketState);
  //printLineToFile(getGPSDataStr());
  //saveSDFile(); // temporary workaround because apparently the file only saves if it gets closed
}

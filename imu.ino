// based on adafruit IMU libarary: //https://github.com/adafruit/Adafruit_BNO055

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/* Set the delay between fresh samples */
uint16_t BNO055_SAMPLERATE_DELAY_MS = 100;

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

void initIMU(void)
{
  /* Initialise the sensor */
  Serial.print("Initializing IMU...");
  while(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.println("no BNO055 detected.");
    delay(100);
    Serial.print("Initializing IMU...");
  }
  Serial.println("done!");
}

void updateIMUData(struct imuStateType &imuState) {
  //could add VECTOR_MAGNETOMETER, VECTOR_GRAVITY...
  sensors_event_t orientationData, angVelocityData, accelData, linearAccelData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&accelData,       Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);

  getIMUEventData(&orientationData, &(imuState.orientation.x),
                  &(imuState.orientation.y), &(imuState.orientation.z));
  getIMUEventData(&angVelocityData, &(imuState.angVel.x),
                  &(imuState.angVel.y), &(imuState.angVel.z));
  getIMUEventData(&accelData, &(imuState.accelWithGravity.x),
                  &(imuState.accelWithGravity.y), &(imuState.accelWithGravity.z));
  getIMUEventData(&linearAccelData, &(imuState.linearAccel.x),
                  &(imuState.linearAccel.y), &(imuState.linearAccel.z));

  imuState.temperature = bno.getTemp();
  
  //delay(BNO055_SAMPLERATE_DELAY_MS);
}

void getIMUEventData(sensors_event_t* event, double* x, double* y, double* z) {
  *x = -1000000; *y = -1000000; *z = -1000000; //dumb values, easy to spot problem
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {
    *x = event->acceleration.x;
    *y = event->acceleration.y;
    *z = event->acceleration.z;
  }
  else if (event->type == SENSOR_TYPE_ORIENTATION) {
    *x = event->orientation.x;
    *y = event->orientation.y;
    *z = event->orientation.z;
  }
  else if (event->type == SENSOR_TYPE_MAGNETIC_FIELD) {
    *x = event->magnetic.x;
    *y = event->magnetic.y;
    *z = event->magnetic.z;
  }
  else if ((event->type == SENSOR_TYPE_GYROSCOPE) || (event->type == SENSOR_TYPE_ROTATION_VECTOR)) {
    *x = event->gyro.x;
    *y = event->gyro.y;
    *z = event->gyro.z;
  }
}

void printIMUData(void)
{
  //could add VECTOR_MAGNETOMETER, VECTOR_GRAVITY...
  sensors_event_t orientationData, angVelocityData, accelData, linearAccelData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&accelData,       Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);

  Serial.println();
  Serial.print("orientation: ");
  printIMUEvent(&orientationData);
  Serial.print("angVel: ");
  printIMUEvent(&angVelocityData);
  Serial.print("accelWithGrav: ");
  printIMUEvent(&accelData);       // with gravity
  Serial.print("accelNoGrav: ");
  printIMUEvent(&linearAccelData); // without gravity

  int8_t boardTemp = bno.getTemp();
  Serial.print(F("temperature: "));
  Serial.println(boardTemp);
  
  delay(BNO055_SAMPLERATE_DELAY_MS);
}

void printIMUEvent(sensors_event_t* event) {
  double x, y, z;
  getIMUEventData(event, &x, &y, &z);
  
  Serial.println();
  Serial.print(event->type);
  Serial.print(": x= ");
  Serial.print(x);
  Serial.print(" | y= ");
  Serial.print(y);
  Serial.print(" | z= ");
  Serial.println(z);
}

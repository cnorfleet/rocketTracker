// modified from: https://www.arduino.cc/en/Tutorial/ReadWrite

#include <SPI.h>
#include <SD.h>

#define MAX_PATH_LENGTH 20
File myFile;
int fileIdx = -1;
String fileName;
char* filePath = new char[MAX_PATH_LENGTH];

#define GPS_LOG_HEADER_STRING "aa"
#define IMU_LOG_HEADER_STRING "Orientation X (deg),Orientation Y (deg),Orientation Z (deg),Ang Vel X,Ang Vel Y,Ang Vel Z,Accel X,Accel Y,Accel Z,Lin Accel X,Lin Accel Y,Lin Accel Z,Temperature (deg F)"

uint32_t timerSD = millis();

void initSD() {
  Serial.print("Initializing SD card...");

  if (!SD.begin()) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  while(SD.exists(nextFileName())) {
    Serial.println(fileName + " exists.");
  }
  Serial.println(fileName + " doesn't exist.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  openFile();

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing header to " + fileName + "...");
    myFile.println(IMU_LOG_HEADER_STRING);
    closeFile();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening " + fileName);
    while(1);
  }
}

void printStatusToFile(struct rocketStateType &rocketState) {
  // if millis() or timer wraps around, we'll just reset it
  if (timerSD > millis()) timerSD = millis();

  // approximately every 2 seconds or so, random intervals, print out the current stats
  static unsigned nextInterval = 2000;
  if (millis() - timerSD > nextInterval) {
    timerSD = millis(); // reset the timer
    nextInterval = 1500 + random(1000);

    openFile();
//    printLineToFile(getGPSDataStr(rocketState.gpsState));
    printLineToFile(getIMUDataStr(rocketState.imuState));
    closeFile();
    }
}

String getIMUDataStr(const struct imuStateType &imuState) {
  sensors_event_t orientationData, angVelocityData, accelData, linearAccelData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&accelData,       Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  sensors_event_t* data[] = { &orientationData, &angVelocityData, &accelData, &linearAccelData };
  int8_t boardTemp = bno.getTemp();

  String out = "";
  for(int i = 0; i < 4; i++) {
    double x, y, z;
    getIMUEventData(data[i], &x, &y, &z);
    out = out + String(x) + "," + String(y) + "," + String(z) + ",";
  }
  out = out + String(boardTemp);
  return out;
}

void printLineToFile(String msg) {
  if(myFile) {
    myFile.println(msg);
  } else {
    Serial.println("error printing to " + fileName);
  }
}

void printFileToSerial() {
  myFile.close();
  delay(1);
  myFile = SD.open(filePath);
  if (myFile) {
    Serial.println(fileName + ":");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening " + fileName);
  }
  myFile.close();
  delay(1);
  myFile = SD.open(filePath, FILE_WRITE);
}

void openFile() {
  myFile = SD.open(filePath, FILE_WRITE);
}

void closeFile() {
  myFile.close();
}

void saveSDFile(void) {
  // temporary workaround because apparently the file only saves if it gets closed
  closeFile();
  delay(1);
  openFile();
}

char* nextFileName() {
  fileIdx++;
  fileName = "log";
  if(fileIdx < 10) fileName = fileName + "0";
  if(fileIdx < 100) fileName = fileName + "0";
  fileName = fileName + String(fileIdx);
  fileName = fileName + ".csv";
  fileName.toCharArray(filePath, MAX_PATH_LENGTH);
  return filePath;
}

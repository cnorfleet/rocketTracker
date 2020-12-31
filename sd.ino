// modified from: https://www.arduino.cc/en/Tutorial/ReadWrite

#include <SPI.h>
#include <SD.h>

#define MAX_PATH_LENGTH 20
File myFile;
int fileIdx = -1;
String fileName;
char* filePath = new char[MAX_PATH_LENGTH];
boolean fileError = false;

#define GPS_LOG_HEADER_STRING "Date,Time (UTC),Year,Month,Day,Hour,Minute,Second,Sat Fix,Quality,Seconds Since Last Fix,Number of Satellites,Latitude,Longitude,Location String,Speed (Knots),Angle,Altitude (meters)"
#define IMU_LOG_HEADER_STRING "Orientation X (deg),Orientation Y (deg),Orientation Z (deg),Ang Vel X,Ang Vel Y,Ang Vel Z,Accel X,Accel Y,Accel Z,Lin Accel X,Lin Accel Y,Lin Accel Z,Temperature (deg F)"
#define RKT_LOG_HEADER_STRING "Battery Voltage (V)"

void initSD() {
  Serial.print("Initializing SD card...");

  while (!SD.begin()) {
    Serial.println("initialization failed!");
    delay(100);
    Serial.print("Initializing SD card...");
  }
  Serial.println("initialization done!");

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
    myFile.print(GPS_LOG_HEADER_STRING);
    myFile.print(",");
    myFile.print(IMU_LOG_HEADER_STRING);
    myFile.print(",");
    myFile.println(RKT_LOG_HEADER_STRING);
    closeFile();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening " + fileName);
    while(1);
  }
}

void printStatusToFile(struct rocketStateType &rocketState) {
  if(fileError) {
    SD.begin();
    delay(1);
  }
  openFile();
  if(myFile) {
    fileError = false;
    myFile.print(getGPSDataStr(rocketState.gpsState) + ",");
    myFile.print(getIMUDataStr(rocketState.imuState) + ",");
    myFile.println(getOtherDataStr(rocketState));
  } else {
    fileError = true;
  }
  closeFile();
}

String getGPSDataStr(const struct gpsStateType &gpsState) {
  String out = "";
  out = out + gpsState.date + "," + gpsState.timeUTC + ",";
  out = out + String(gpsState.gpsTime.yr) + "," + String(gpsState.gpsTime.mth) +
        "," + String(gpsState.gpsTime.d)  + "," + String(gpsState.gpsTime.h) +
        "," + String(gpsState.gpsTime.m)  + "," + String(gpsState.gpsTime.s) + ",";
  out = out + String(gpsState.satFix) + "," + String(gpsState.quality) +
        "," + String(gpsState.secondsSinceFix) +
        "," + String(gpsState.numSats) + ",";
  out = out + ((gpsState.lat=='N') ? "+" : "-") + String(gpsState.latitude, 4) +
        "," + ((gpsState.lon=='E') ? "+" : "-") + String(gpsState.longitude,4) + ",";
  out = out + gpsState.location + ",";
  out = out + String(gpsState.speedKnots) +
        "," + String(gpsState.angle) +
        "," + String(gpsState.altitude);
  return out;
}

String getIMUDataStr(const struct imuStateType &imuState) {
  threeAxisDataType data[] = { imuState.orientation,
                               imuState.angVel,
                               imuState.accelWithGravity,
                               imuState.linearAccel};
  String out = "";
  for(int i = 0; i < 4; i++) {
    out = out + String(data[i].x) + "," +
                String(data[i].y) + "," +
                String(data[i].z) + ",";
  }
  out = out + String(imuState.temperature);
  return out;
}

String getOtherDataStr(const struct rocketStateType &rocketState) {
  return String(rocketState.batteryVoltage);
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

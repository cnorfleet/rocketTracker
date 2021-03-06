void initSerialPrinter(void) {
  Serial.begin(9600);
  unsigned long startTime = millis();
  while (!Serial) {
    // wait for serial port to connect
    if(startTime - millis() > 1000) {
      // if taking too long we should probably just move on
      return;
    }
  }
}

void printStatusToSerial(struct rocketStateType &rocketState) {
  Serial.println();
  Serial.println("File name: " + fileName +
                 String(fileError ? " - error writing to file" : ""));
  Serial.println("Battery voltage: " +
                 String(rocketState.batteryVoltage, 4) + " V");
  Serial.println();
  printGPSData(rocketState.gpsState);
  Serial.println();
  printIMUData(rocketState.imuState);
  Serial.println();
}

void printGPSData(const struct gpsStateType &gpsState) {
  // Time in seconds keeps increasing after we get the NMEA sentence.
  // This estimate will lag real time due to transmission and parsing delays,
  // but the lag should be small and should also be consistent.
  float s = GPS.seconds + GPS.milliseconds/1000. + GPS.secondsSinceTime(); 
  int m = GPS.minute;
  int h = GPS.hour;
  int d = GPS.day;
  // Adjust time and day forward to account for elapsed time.
  // This will break at month boundaries! Humans will have to cope with April 31,32 etc.
  while(s > 60){ s -= 60; m++; }
  while(m > 60){ m -= 60; h++; }
  while(h > 24){ h -= 24; d++; }
  // ISO Standard Date Format, with leading zeros https://xkcd.com/1179/ 
  Serial.println("Date: " + gpsState.date +
              "   Time: " + gpsState.timeUTC);
  Serial.println("Fix: " + String(gpsState.satFix == 1 ? "yes," : "no, ") +
                " quality: " + String(gpsState.quality) +
                ", satellites: " + String(gpsState.numSats));
  Serial.println("Times [s] since last fix: " + String(gpsState.secondsSinceFix));
  Serial.println("Location: " + gpsState.location);
  Serial.println("Speed (knots): " + String(gpsState.speedKnots));
  //Serial.println("Angle (degrees): " + String(gpsState.angle));
  Serial.println("Altitude (meters): " + String(gpsState.altitude));
}

String formatThreeDigits(double d) {
  double absD = abs(d);
  return (String(   d > 0   ? "+" : "-") +
          String(absD < 100 ? "0" : "") +
          String(absD < 10  ? "0" : "") +
          (absD < 0.05 ? "0.0" : String(absD, 1)));
}

String threeAxisToString(const struct threeAxisDataType data) {
  if(data.x == -1000000 and data.y == -1000000 and data.z == -1000000)
    return("No Data");
  return ("x= " + formatThreeDigits(data.x) + 
       " | y= " + formatThreeDigits(data.y) +
       " | z= " + formatThreeDigits(data.z));
}

void printIMUData(const struct imuStateType &imuState) {
  Serial.println("Orientation (deg): " + threeAxisToString(imuState.orientation));
  Serial.println("Ang Vel (deg/s):   " + threeAxisToString(imuState.angVel));
  Serial.println("Accel w/  G (m/s): " + threeAxisToString(imuState.accelWithGravity));
  Serial.println("Accel w/o G (m/s): " + threeAxisToString(imuState.linearAccel));
  Serial.println("Temperature: " + String(imuState.temperature) + " degrees C");
}

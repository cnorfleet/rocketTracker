// Based on GPS_HardwareSerial_Timing example code

#include <Adafruit_GPS.h>

#define GPSSerial Serial1
Adafruit_GPS GPS(&GPSSerial);

// Echo the raw GPS sentences to the Serial console
#define GPSECHO false
#define GPSDEBUG false

void initGPS(void) {
  Serial.print("Initializing GPS...");
  
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  
  // Set the update rate (uncomment the one you want.)
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  //GPS.sendCommand(PMTK_SET_NMEA_UPDATE_200_MILLIHERTZ); // 5 second update time
  //GPS.sendCommand(PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ); // 10 second update time
  
  // Request updates on antenna status, comment out to keep quiet
  //GPS.sendCommand(PGCMD_ANTENNA);
  
  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
  
  Serial.println("done!");
}

inline void adjustTimeAndDate(int& yr, int& mth, int& d, int& h, int& m, float& s) {
  // Adjust time and day forward to account for elapsed time.
  // Currently this will break at month boundaries (April 31,32 etc.)
  while(s > 60){ s -= 60; m++; }    
  while(m > 60){ m -= 60; h++; }
  while(h > 24){ h -= 24; d++; }
}

void updateGPSData(struct gpsStateType &gpsState) {
  char c = GPS.read();
  if (GPSECHO)
    if (c) Serial.print(c);
  
  if (GPS.newNMEAreceived()) {
    if(GPSDEBUG) Serial.println("recievedNew");
    //Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) { // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
      if(GPSDEBUG) Serial.println("failed");
    }
    if(GPSDEBUG) Serial.println("parsed");
  }
  
  // Time in seconds keeps increasing after we get the NMEA sentence.
  // This estimate will lag real time due to transmission and parsing delays,
  // but the lag should be small and should also be consistent.
  float& s = gpsState.gpsTime.s;
  int& m   = gpsState.gpsTime.m;
  int& h   = gpsState.gpsTime.h;
  int& d   = gpsState.gpsTime.d;
  int& mth = gpsState.gpsTime.mth;
  int& yr  = gpsState.gpsTime.yr;
  
  s = GPS.seconds + GPS.milliseconds/1000. + GPS.secondsSinceTime(); 
  m = GPS.minute;
  h = GPS.hour;
  d = GPS.day;
  mth = GPS.month;
  yr  = GPS.year;
  adjustTimeAndDate(yr, mth, d, h, m, s);
  
  // ISO Standard Date Format, with leading zeros https://xkcd.com/1179/
  gpsState.date = String(yr+2000) + "-" +
                      ((mth < 10) ? "0" : "") + String(mth) + "-" +
                      ((d   < 10) ? "0" : "") + String(d);
  gpsState.timeUTC =  ((h   < 10) ? "0" : "") + String(h) + ":" +
                      ((m   < 10) ? "0" : "") + String(m) + ":" +
                      ((s   < 10) ? "0" : "") + String(s) + " (UTC)";
  gpsState.satFix  = (int)GPS.fix;
  gpsState.quality = (int)GPS.fixquality;
  gpsState.numSats = GPS.satellites;
  if (GPS.fix) {
    gpsState.latitude = GPS.latitude;
    gpsState.lat = GPS.lat;
    gpsState.longitude = GPS.longitude;
    gpsState.lon = GPS.lon;
    gpsState.location = String(gpsState.latitude/100, 0) + " " +
                        String(fmod(gpsState.latitude, 100), 4) + "\' " +
                        String(gpsState.lat) + " " +
                        String(gpsState.longitude/100, 0) + " " +
                        String(fmod(gpsState.longitude, 100.0), 4) + "\' " +
                        String(gpsState.lon);
    gpsState.speedKnots = GPS.speed;
    gpsState.angle = GPS.angle;
    gpsState.altitude = GPS.altitude;
    gpsState.secondsSinceFix = 0;
  } else {
    gpsState.secondsSinceFix = GPS.secondsSinceFix();
  }
}

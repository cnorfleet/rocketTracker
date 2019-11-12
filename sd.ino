// modified from: https://www.arduino.cc/en/Tutorial/ReadWrite

#include <SPI.h>
#include <SD.h>

#define MAX_FILE_LENGTH 20
File myFile;
int fileIdx = -1;
String fileName;
char* filePath = new char[MAX_FILE_LENGTH];

#define headerStr "testing 1, 2, 3."

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
  myFile = SD.open(filePath, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing header to " + fileName + "...");
    myFile.println(headerStr);
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening " + fileName);
    while(1);
  }
}

void printLineToFile(char* msg) {
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

void closeFile() {
  myFile.close();
}

char* nextFileName() {
  fileIdx++;
  fileName = "log";
  if(fileIdx < 10) fileName = fileName + "0";
  if(fileIdx < 100) fileName = fileName + "0";
  fileName = fileName + String(fileIdx);
  fileName = fileName + ".txt";
  fileName.toCharArray(filePath, MAX_FILE_LENGTH);
  return filePath;
}

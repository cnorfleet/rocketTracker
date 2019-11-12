// modified from: https://www.arduino.cc/en/tutorial/files

#define MAX_FILE_LENGTH 10
File file;
int fileIdx = -1;
String fileName;
char* filePath;

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

  // open a new file and immediately close it:
  Serial.println("Creating " + fileName + "...");
  file = SD.open(filePath, FILE_WRITE);
  file.println("test");
  file.close();

  // testing:
  Serial.println("opening file ");

  // Check to see if the file exists:
  if (SD.exists(filePath)) {
    Serial.println(fileName + " exists.");
  } else {
    Serial.println(fileName + " doesn't exist.");
  }
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

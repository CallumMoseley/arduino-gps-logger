#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SD.h>

static const int RXPin = 3, TXPin = 2;
static const uint32_t GPSBaud = 4800;
static const int buttonPin = 5;
int prevButton = HIGH;
boolean gpsAcquired = false;
boolean recording = false;
File curFile;
long lastPoint = 0;

TinyGPSPlus gps;

SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  // Begin GPS serial communications
  ss.begin(GPSBaud);
  
  // Pin setup
  pinMode(10, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
  SD.begin(10);
  
  // Wait until GPS satellites acquired
  while (!gps.date.isValid() || !gps.time.isValid() || !gps.location.isValid() || !gps.altitude.isValid())
  {
    // Read in from GPS
    while (ss.available())
      gps.encode(ss.read());
  }
  
}

void loop()
{
  // Checks time since last reading.  If it is too long, the GPS has lost signal and needs to re-acquire it
  if (gps.location.age() > 1500)
  {
    gpsAcquired = false;
  }
  else
  {
    gpsAcquired = true;
  }
  
  // Read button state
  int buttonState = digitalRead(buttonPin);
  boolean pushed = false;
  if (buttonState == LOW && prevButton == HIGH)
  {
    pushed = true;
  }
  if (pushed)
  {
    if (!recording)
    {
      // Begin recording data
      recording = true;
      char curYear[5];
      char path[8];
      char filename[21];
      sprintf(curYear, "%d\0", gps.date.year());
      sprintf(path, "%d/%02d\0", gps.date.year(), gps.date.month());
      sprintf(filename, "%d/%02d/%02d%02d%02d%02d.gpx\0", gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());
      if (!SD.exists(curYear))
      {
        SD.mkdir(curYear);
      }
      if (!SD.exists(path))
      {
        SD.mkdir(path);
      }
      if (SD.exists(filename))
      {
        SD.remove(filename);
      }
      curFile = SD.open(filename, FILE_WRITE);
      curFile.println("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>");
      curFile.println("<gpx>");
      curFile.println(" <metadata>");
      curFile.print  ("  <time>"); curFile.printf("%4d-%02d-%02dT%02d:%02d:%02dZ", gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second()); curFile.println("</time>");
      curFile.println(" </metadata>");
      curFile.println(" <trk>");
      curFile.print  ("  <name>"); curFile.printf("%4d-%02d-%02dT%02d:%02d:%02dZ", gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second()); curFile.println("</name>");
      curFile.println("  <trkseg>");
    }
    else
    {
      // End recording and close file
      recording = false;
      curFile.println("  </trkseg>");
      curFile.println(" </trk>");
      curFile.println("</gpx>");
      curFile.close();
    }
    
  }
  
  if (recording && gpsAcquired && millis() - lastPoint >= 1000)
  {
    lastPoint = millis();
    curFile.print  ("   <trkpt lat=\""); curFile.print(gps.location.lat(), 7); curFile.print("\" lon=\""); curFile.print(gps.location.lng(), 7); curFile.println("\">");
    curFile.print  ("    <ele>"); curFile.print(gps.altitude.meters(), 2); curFile.println("</ele>");
    curFile.print  ("    <time>"); curFile.printf("%4d-%02d-%02dT%02d:%02d:%02dZ", gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second()); curFile.println("</time>");
    curFile.println("   </trkpt>");
  }
  
  while (ss.available())
    gps.encode(ss.read());
    
  prevButton = buttonState;
}

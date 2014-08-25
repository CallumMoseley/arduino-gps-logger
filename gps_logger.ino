#include "SD.h"
#include "TinyGPS++.h"
#include "SoftwareSerial.h"
#define BUTTON 7
#define LED 2

File curFile;
TinyGPSPlus gps;
int prevButton = 0;
boolean recording = false;
int counter = 0;
SoftwareSerial ss(4, 3);
boolean gpsAquired = false;

void setup()
{
  Serial.begin(115200);
  pinMode(10, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);
  ss.begin(4800);
  //SD.begin(10);
}

void loop()
{
  Serial.println(gpsAquired);
  /*if (gps.location.age() > 1500) {
    gpsAquired = false;
  } else {
    gpsAquired = true;
  }*/
  while (ss.available())
    gps.encode(ss.read());
  int buttonState = digitalRead(BUTTON);
  boolean pushed = false;
  if (buttonState == LOW && prevButton != buttonState)
  {
    pushed = true;
  }
  if (pushed)
  {
    if (recording)
    {
      curFile.print("  </trkseg>\n"
                    " </trk>\n"
                    "</gpx>\n");
      digitalWrite(LED, LOW);
      curFile.close();
    }
    else
    {
      String fileName = timeString() + ".gpx";
      char* fileNameChar;
      fileName.toCharArray(fileNameChar, fileName.length());
      curFile = SD.open(fileNameChar, FILE_WRITE);
      curFile.println("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>");
      curFile.println("<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" xmlns:gpxx=\"http://www.garmin.com/xmlschemas/GpxExtensions/v3\" xmlns:gpxtpx=\"http://www.garmin.com/xmlschemas/TrackPointExtension/v1\" creator=\"Callum's Super Awesome GPS\" version=\"1.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd http://www.garmin.com/xmlschemas/GpxExtensions/v3 http://www.garmin.com/xmlschemas/GpxExtensionsv3.xsd http://www.garmin.com/xmlschemas/TrackPointExtension/v1 http://www.garmin.com/xmlschemas/TrackPointExtensionv1.xsd\">");
      curFile.println(" <metadata>");
      curFile.println("  <time>" + timeString() + "</time>");
      curFile.println(" </metadata>");
      curFile.println(" <trk>");
      curFile.println("  <name>" + timeString() + "</name>");
      curFile.println("  <trkseg>");
      digitalWrite(LED, HIGH);
    }
  }
  prevButton = buttonState;
  if (recording && counter == 5000000 && gpsAquired)
  {
    counter = 0;
    curFile.print("   <trkpt lat=\""); curFile.print(gps.location.lat(), 6); curFile.print("\" lon=\""); curFile.print(gps.location.lng(), 6); curFile.println("\">");
    curFile.println("    <ele>"); curFile.print(gps.altitude.meters(), 6); curFile.println("</ele>");
    curFile.println("    <time>" + timeString() + "</time>");
    curFile.println("   </trkpt>");
  }
  counter = counter % 5000000;
  counter++;
}

String timeString()
{
  return (String)(String(gps.date.year()) + "-" + String(gps.date.month()) + "-" + String(gps.date.day()) + "T" + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second()) + "Z");
}

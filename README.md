arduino-gps-logger
==================

A simple Arduino sketch for tracking GPS data and logging it to a .gpx on a microSD.

# Hardware

The Arduino I used in this project was an Arduino Pro Mini 3.3 V.  The GPS module was an EM-406A.  The device was powered by a 400 mAh battery which was charged by a 0.5 W solar panel on top of the device.  The enclosure was 3D printed.

# File system

The files are stored by the date and time they were started as YYYY/MM/DD/hhmmss.gpx.

# Solar Panel Tracker Project

Embedded systems project featuring a Raspberry Pi 3 and a solar tracking device. A two-axis (pan & tilt) platform is used to track the sun and the information it gathers is displayed on an HMI. The basic control is as follows:

  1. The position of the earth and the current time is used to calculate the sun's position
  2. The panel is moved to face the sun's calculated position
  3. This position is adjusted incrementally as information is gathered from an array of sensor on the tracker
 
 
## HMI

The HMI is built using GLGToolkit and includes a variety of measurements and parameters provided by the development platform. These parameters include:
  * Panel azimuth and elevation
  * Panel position (latitude and logitude)
  * Date and time
  * Light intensity (lux)
  * Temperature, humidity and pressure
  * Wind speed and direction
  * Tracking status
  * GPS status
  
## Sensors

There are two main sensors used in this project; the Adafruit TSL2561 and the Bosch BME280. Both are integrated on to a PCB which attached to the Raspberry PI GPIO pins.

## Third-party tools

A variety of third party programs and tools are used in this project. They are as follows:

  * GLGToolkit HMI Graphics 
  * Solar position algorithms and calculations from the NREL (spa.h/c)
  * TSL2561 library (tsl2561.h/c)
  * GPS/NMEA parsing code (gps.h/c, nmea.h/c)
  * BME280 library (hshbme280.h/c)
  * wiringPi library
  
## Building

The included makefile can be run with GNU make to build the entire project. 

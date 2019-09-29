/** \file panel.h
 *  \brief header file for panel.c
 * \author Created: Thomas Aziz
 * \date 24JAN019
*/
#ifndef PANEL_H
#define PANEL_H
#include "wxstn.h"
#include "gps.h"

// Panel Position Constants

//wiringPi device pinbase
#define ST_PCF8591_I2CADR   0x48
#define ST_RTC_I2CADR       0x68
#define ST_PCF8591_PINBASE  200

// Panel Position constants
#define STSIMPOS 1

// Theoretical Limits
#define STMAXEL     90.0
#define STMINEL     0.0
#define STMAXAZ     360.0
#define STMINAZ     0.0

// Hardware Limits
#define STMAXELDEG  90.0
#define STMINELDEG  0.0
#define STMAXAZDEG  315.0
#define STMINAZDEG  45.0

// Servo Constants
#define STELPIN     1
#define STSERVORANGE  90.0     // Servo maximum travel 90 degrees
#define STPWMCLOCK  384
#define STPWMRANGE  1000
#define STPWMMAX    105.0
#define STPWMMIN    40.0         // 5.0 lowest value offset 30 degrees up, 45.0
#define STPWMDELAY  250

// Stepper Constants
#define STAZSTEP    2
#define STAZDIR     3
#define STSTEPRANGE 360.0
#define STSTEPMAX   1000
#define STSTEPMIN   0
#define STSTEPDELAY 1

// Position Feedback Constants
#define STPE000     45.0        // Low end offset by 30 degrees
#define STPE090     120.0
#define STPA000     0.0
#define STPA360     255.0
#define STSAVEPRD   1

#define STMAXPSZ    360

// Position Table Constants
#define STNOTABLE	0
#define STPOSTBLTXT	1
#define STMAXTBLSZ	361
#define STMAXSTEPSZ	360
#define STMAXSERVSZ 90

#define SIMTRACK 1

#define DELTAUT1 0.0
#define DELTAT 67.0
#define DTIMEZONE -4.0
#define DLATITUDE 43.63409
#define DLONGITUDE -79.45930
#define DALTITUDE 166.0
#define DPANELHDG 0.0
#define PAZIMUTH 180.0
#define PELEVATION 0.0
#define DSLOPE 0.0
#define DAZROT 0.0
#define DATMREF 0.5667

//LDR Constants
#define SIMLDR 0
#define STSECTR 127
#define STSACTR 127

#define STLEDMIN 0
#define STLEDMAX 255



// Data structures
typedef struct panelpos
{
    double Azimuth;
    double Elevation;
	loc_t gpsdata;
} panelpos_s;

typedef struct ldrsensor
{
    int aset;
    int eset;
} ldrsensor_s;

typedef struct positiondata
{
	int apos;	///< Analog azimuth position
	int cnt;	///< Azimuth stepper count
	int epos;	///< Analog elevation position
	int pwm;	///< Elevation pwm value
} positiondata_s;

typedef struct paneldata
{
    double azimuth;
    double elevation;
    double latitude;
    double longitude;
} paneldata_s;


// Function Prototypes
int StPanelInitialization(void);
void StServoSetup(void);
void StStepperSetup(void);
ldrsensor_s StGetLdrReadings(void);
void StDisplayLdrReadings(ldrsensor_s dsens);
panelpos_s StGetPanelPosition(void);
void StSetPanelPosition(panelpos_s newpos);
int StSavePositionTable(void);
int StRetrievePositionTable(void);
void StSetCalibrationLED(unsigned short);
int StLogPanelData(paneldata_s pdata, reading_s creads);


#endif // PANEL_H

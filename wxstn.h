/** \file wxstn.h
 *  \brief header file for weatherstation functions
 * \author Created: Thomas Aziz
 * \date 24JAN019
*/

#ifndef WXSTN_H
#define WXSTN_H

#include <time.h>

// Constants

// Simulation Constants
#define SIMULATE 1
#define SIMTEMP 0
#define SIMHUMID 0
#define SIMPRESS 0
#define SIMLIGHT 1
#define SIMWINDSPD 1
#define SIMWINDDIR 1
#define USTEMP 50
#define LSTEMP -10
#define USHUMID 100
#define LSHUMID 0
#define USPRESS 1016
#define LSPRESS 985
#define USLIGHT 255
#define LSLIGHT 0
#define USWINDSPD 100
#define LSWINDSPD 0
#define USWINDDIR 360
#define LSWINDDIR 0

// Structures
typedef struct readings
{
	time_t rtime;			///<Reading time
	double temperature;		///<degrees Celsius
	double humidity;		///<percent relative humidity
	double pressure;		///<milliBars
    double light;			///<relative intensity 0-255
	double windspeed;		///<kph
	double winddirection;	///<degrees True
}reading_s;


// Function Prototypes
int WsInit(void);
reading_s WsGetReadings(void);
double WsGetTemperature(void);
double WsGetHumidity(void);
double WsGetPressure(void);
double WsGetLight(void);
double WsGetWindspeed(void);
double WsGetWinddirection(void);
int WsGetRandom(int range);
void WsDelay(int milliseconds);
double PaTomB(double pa);
void WsDisplayReadings(reading_s dreads);
#endif
